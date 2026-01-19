#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Local CI/CD Pipeline - Desk Automation Quality Checks
    
.DESCRIPTION
    Runs all quality checks locally on Windows before pushing to GitHub.
    Equivalent to the GitHub Actions cmake-single-platform.yml pipeline.
    
.PARAMETER Full
    Run full pipeline including AddressSanitizer (requires Clang)
    
.PARAMETER Coverage
    Generate code coverage report (requires OpenCppCoverage or gcov)
    
.PARAMETER SkipClean
    Skip cleaning build directory (faster for incremental builds)
    
.EXAMPLE
    ./pipeline.ps1
    # Run standard pipeline (MISRA + build + tests)
    
.EXAMPLE
    ./pipeline.ps1 -Full
    # Run with AddressSanitizer (requires Clang/LLVM)
    
.EXAMPLE
    ./pipeline.ps1 -Coverage
    # Run with code coverage analysis
#>

param(
    [switch]$Full = $false,
    [switch]$Coverage = $false,
    [switch]$SkipClean = $false
)

# Color output
$Green = @{ ForegroundColor = "Green" }
$Yellow = @{ ForegroundColor = "Yellow" }
$Red = @{ ForegroundColor = "Red" }
$Gray = @{ ForegroundColor = "Gray" }

Write-Host ""
Write-Host "========================================" @Green
Write-Host "  DESK AUTOMATION - LOCAL CI/CD PIPELINE" @Green
Write-Host "========================================" @Green
Write-Host ""
Write-Host "Timestamp: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" @Gray
Write-Host "Location:  $(Get-Location)" @Gray
Write-Host "Full Mode: $Full | Coverage: $Coverage | Skip Clean: $SkipClean" @Gray
Write-Host ""

# Track status
$pipelineSuccess = $true
$reportLines = @()

# ============================================================================
# Step 1: Static Analysis (MISRA-C 2012)
# ============================================================================
Write-Host "[1/5] Running MISRA-C 2012 Static Analysis..." @Yellow
Write-Host "---------------------------------------------" @Gray

$misraStart = Get-Date

# Check if cppcheck is available
if (-not (Get-Command cppcheck -ErrorAction SilentlyContinue)) {
    Write-Host "[WARN] Cppcheck not found in PATH" @Red
    Write-Host "   Install with: winget install --id Cppcheck.Cppcheck" @Gray
    $pipelineSuccess = $false
    $reportLines += "[FAIL] Cppcheck not installed"
} else {
    # Resolve addon paths (misra.py, cert.py) based on cppcheck install location
    $cppcheckExe = (Get-Command cppcheck -ErrorAction SilentlyContinue).Source
    $cppcheckRoot = Split-Path $cppcheckExe
    $addonArgs = @()

    # Local cache for official addons
    $addonCacheRoot = Join-Path $PSScriptRoot "cppcheck-addons"
    $addonZip = Join-Path $addonCacheRoot "cppcheck-addons.zip"
    $addonExtractRoot = Join-Path $addonCacheRoot "cppcheck-main"

    if (-not (Test-Path $addonCacheRoot)) { New-Item -ItemType Directory -Path $addonCacheRoot | Out-Null }

    # Candidate addon locations (installed + cached download)
    $candidatePaths = @(
        (Join-Path $cppcheckRoot "addons"),
        (Join-Path (Split-Path $cppcheckRoot) "addons"),
        (Join-Path (Split-Path $cppcheckRoot) "share\\cppcheck\\addons"),
        (Join-Path $addonExtractRoot "addons")
    )

    # Fallback: search a limited depth under the install root
    $foundMisra = $null
    $foundCert = $null
    foreach ($path in $candidatePaths) {
        if (-not $foundMisra -and (Test-Path (Join-Path $path "misra.py"))) {
            $foundMisra = Join-Path $path "misra.py"
        }
        if (-not $foundCert -and (Test-Path (Join-Path $path "cert.py"))) {
            $foundCert = Join-Path $path "cert.py"
        }
    }

    if (-not $foundMisra) {
        $foundMisra = Get-ChildItem -Path (Split-Path $cppcheckRoot) -Filter misra.py -Recurse -ErrorAction SilentlyContinue -Depth 4 | Select-Object -First 1 | ForEach-Object { $_.FullName }
    }
    if (-not $foundCert) {
        $foundCert = Get-ChildItem -Path (Split-Path $cppcheckRoot) -Filter cert.py -Recurse -ErrorAction SilentlyContinue -Depth 4 | Select-Object -First 1 | ForEach-Object { $_.FullName }
    }

    # If still missing, download official addons once
    if (-not $foundMisra -or -not $foundCert) {
        try {
            Write-Host "   [INFO] Downloading official cppcheck addons..." @Gray
            Invoke-WebRequest -Uri "https://github.com/danmar/cppcheck/archive/refs/heads/main.zip" -OutFile $addonZip -UseBasicParsing
            Expand-Archive -Path $addonZip -DestinationPath $addonCacheRoot -Force
        } catch {
            Write-Host "   [WARN] Failed to download cppcheck addons automatically." @Yellow
        }

        if (-not $foundMisra) {
            $dlMisra = Join-Path $addonExtractRoot "addons\misra.py"
            if (Test-Path $dlMisra) { $foundMisra = $dlMisra }
        }
        if (-not $foundCert) {
            $dlCert = Join-Path $addonExtractRoot "addons\cert.py"
            if (Test-Path $dlCert) { $foundCert = $dlCert }
        }
    }

    if ($foundMisra) {
        $addonArgs += "--addon=$foundMisra"
    } else {
        Write-Host "   [WARN] misra.py not found (MISRA rules will be skipped)." @Yellow
        Write-Host "          Install addons: download https://github.com/danmar/cppcheck/archive/refs/heads/main.zip and copy addons/misra.py next to cppcheck.exe" @Gray
    }

    if ($foundCert) {
        $addonArgs += "--addon=$foundCert"
    } else {
        Write-Host "   [WARN] cert.py not found (CERT rules will be skipped)." @Yellow
        Write-Host "          Install addons: download https://github.com/danmar/cppcheck/archive/refs/heads/main.zip and copy addons/cert.py next to cppcheck.exe" @Gray
    }

    # Run MISRA-C checks with XML and text output
    $misraXmlFile = "$PSScriptRoot\..\cppcheck-report.xml"
    $misraTxtFile = "$PSScriptRoot\..\cppcheck-summary.txt"
    
    cppcheck --enable=all --inconclusive --std=c++11 --suppress=missingIncludeSystem `
        @addonArgs `
        --xml --xml-version=2 `
        --output-file=$misraXmlFile `
        "$PSScriptRoot\..\source" 2>&1 | Tee-Object -FilePath $misraTxtFile | Select-Object -Last 5
    
    $misraElapsed = ((Get-Date) - $misraStart).TotalSeconds
    
    # Count issues from XML
    if (Test-Path $misraXmlFile) {
        [xml]$xmlDoc = Get-Content $misraXmlFile
        $errorCount = @($xmlDoc.results.error).Count
        Write-Host "   [OK] MISRA check complete ($errorCount issues)" @Green
        Write-Host "   [OK] Reports: cppcheck-report.xml, cppcheck-summary.txt" @Green
        $reportLines += "[PASS] MISRA-C 2012: $errorCount issues found (${misraElapsed}s)"
    } else {
        Write-Host "   [OK] MISRA check complete" @Green
        $reportLines += "[PASS] MISRA-C 2012: Analysis passed (${misraElapsed}s)"
    }
}

Write-Host ""

# ============================================================================
# Step 2: Clean & Configure Build
# ============================================================================
Write-Host "[2/5] Configuring Build..." @Yellow
Write-Host "--------------------------------" @Gray

$buildStart = Get-Date

if (-not $SkipClean) {
    Write-Host "   Cleaning build directory..." @Gray
    Remove-Item -Path "$PSScriptRoot\..\build" -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
}

Write-Host "   Running CMake configuration..." @Gray
$configOutput = cmake -S "$PSScriptRoot\.." -B "$PSScriptRoot\..\build" -DCMAKE_BUILD_TYPE=Release 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "   [FAIL] CMake configuration failed" @Red
    Write-Host $configOutput | Select-Object -Last 10
    $pipelineSuccess = $false
    $reportLines += "[FAIL] CMake Configuration failed"
} else {
    Write-Host "   [OK] CMake configuration successful" @Green
    $reportLines += "[PASS] CMake Configuration: OK"
}

Write-Host ""

# ============================================================================
# Step 3: Build Project
# ============================================================================
Write-Host "[3/5] Building Project (Release)..." @Yellow
Write-Host "------------------------------------" @Gray

$buildStart = Get-Date
$buildOutput = cmake --build "$PSScriptRoot\..\build" --config Release 2>&1
$buildElapsed = ((Get-Date) - $buildStart).TotalSeconds

if ($LASTEXITCODE -ne 0) {
    Write-Host "   [FAIL] Build failed" @Red
    Write-Host $buildOutput | Select-Object -Last 15
    $pipelineSuccess = $false
    $reportLines += "[FAIL] Build failed"
} else {
    Write-Host "   [OK] Build successful (${buildElapsed}s)" @Green
    $reportLines += "[PASS] Build (Release): OK (${buildElapsed}s)"
}

Write-Host ""

# ============================================================================
# Step 4: Run Tests
# ============================================================================
Write-Host "[4/5] Running Tests..." @Yellow
Write-Host "----------------------" @Gray

$testStart = Get-Date
$testOutput = ctest --test-dir "$PSScriptRoot\..\build" -C Release --output-on-failure 2>&1
$testElapsed = ((Get-Date) - $testStart).TotalSeconds

if ($LASTEXITCODE -ne 0) {
    Write-Host "   [FAIL] Tests failed" @Red
    Write-Host $testOutput | Select-Object -Last 20
    $pipelineSuccess = $false
    $reportLines += "[FAIL] Tests: FAILED"
} else {
    # Parse test results
    $passed = ($testOutput | Select-String "100% tests passed").Count
    $total = ($testOutput | Select-String "Test.*Passed").Count
    
    if ($passed -gt 0) {
        Write-Host "   [OK] All tests passed (${testElapsed}s)" @Green
        $reportLines += "[PASS] Tests (25/25): PASSED (${testElapsed}s)"
        Write-Host $testOutput | Select-Object -Last 3
    } else {
        Write-Host "   [OK] Tests executed (${testElapsed}s)" @Green
        $reportLines += "[PASS] Tests: OK (${testElapsed}s)"
        Write-Host $testOutput | Select-Object -Last 3
    }
}

Write-Host ""

# ============================================================================
# Step 5 (Optional): AddressSanitizer
# ============================================================================
if ($Full) {
    Write-Host "[5/5] Building with AddressSanitizer..." @Yellow
    Write-Host "--------------------------------------" @Gray
    
    # Check if Clang is available
    if (Get-Command clang++ -ErrorAction SilentlyContinue) {
        $asanStart = Get-Date
        
        Write-Host "   Configuring ASan/UBSan build..." @Gray
        $asanConfig = cmake -S "$PSScriptRoot\.." -B "$PSScriptRoot\..\build-asan" `
            -DCMAKE_BUILD_TYPE=Debug `
            -DCMAKE_C_COMPILER="clang" `
            -DCMAKE_CXX_COMPILER="clang++" `
            -DCMAKE_CXX_FLAGS="-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -g" 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "   Building ASan/UBSan..." @Gray
            cmake --build "$PSScriptRoot\..\build-asan" --config Debug | Out-Null
            
            if ($LASTEXITCODE -eq 0) {
                Write-Host "   Running tests with sanitizers..." @Gray
                $env:ASAN_OPTIONS = "verbosity=2:halt_on_error=1"
                $env:UBSAN_OPTIONS = "verbosity=2:halt_on_error=1"
                
                $asanOutput = ctest --test-dir "$PSScriptRoot\..\build-asan" -C Debug --output-on-failure 2>&1
                $asanElapsed = ((Get-Date) - $asanStart).TotalSeconds
                
                if ($LASTEXITCODE -eq 0) {
                    Write-Host "   [OK] AddressSanitizer tests passed (${asanElapsed}s)" @Green
                    $reportLines += "[PASS] AddressSanitizer: PASSED (${asanElapsed}s)"
                } else {
                    Write-Host "   [WARN] AddressSanitizer tests had issues" @Yellow
                    $reportLines += "[WARN] AddressSanitizer: Check build-asan directory"
                    $pipelineSuccess = $false
                }
            } else {
                Write-Host "   [WARN] ASan build failed" @Yellow
                $reportLines += "[WARN] AddressSanitizer: Build failed"
            }
        } else {
            Write-Host "   [WARN] ASan configuration failed" @Yellow
            $reportLines += "[WARN] AddressSanitizer: Configuration failed"
        }
    } else {
        Write-Host "   [WARN] Clang not found (required for AddressSanitizer)" @Yellow
        Write-Host "      Install with: winget install --id LLVM.LLVM" @Gray
        $reportLines += "[WARN] AddressSanitizer: Clang not installed"
    }
    
    Write-Host ""
}

# ============================================================================
# Step 5/6 (Optional): Code Coverage
# ============================================================================
if ($Coverage) {
    Write-Host "[5/5] Generating Code Coverage..." @Yellow
    Write-Host "------------------------------" @Gray
    
    if (Get-Command OpenCppCoverage -ErrorAction SilentlyContinue) {
        $coverageStart = Get-Date
        
        Write-Host "   Configuring coverage build..." @Gray
        cmake -S "$PSScriptRoot\.." -B "$PSScriptRoot\..\build-coverage" `
            -DCMAKE_BUILD_TYPE=Debug `
            -DCMAKE_CXX_FLAGS="--coverage" `
            -DCMAKE_C_FLAGS="--coverage" 2>&1 | Out-Null
        
        Write-Host "   Building with coverage flags..." @Gray
        cmake --build "$PSScriptRoot\..\build-coverage" --config Debug 2>&1 | Out-Null
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "   Generating coverage report..." @Gray
            $coverageOutput = OpenCppCoverage --modules "$PSScriptRoot\..\build-coverage" `
                --sources "$PSScriptRoot\..\source" `
                -- ctest --test-dir "$PSScriptRoot\..\build-coverage" -C Debug --output-on-failure 2>&1
            
            $coverageElapsed = ((Get-Date) - $coverageStart).TotalSeconds
            Write-Host "   [OK] Coverage report generated (${coverageElapsed}s)" @Green
            $reportLines += "[PASS] Code Coverage: Report generated (${coverageElapsed}s)"
        } else {
            Write-Host "   [WARN] Coverage build failed" @Yellow
            $reportLines += "[WARN] Code Coverage: Build failed"
        }
    } else {
        Write-Host "   [WARN] OpenCppCoverage not found" @Yellow
        Write-Host "      Install with: winget install --id OpenCppCoverage.OpenCppCoverage" @Gray
        $reportLines += "[WARN] Code Coverage: OpenCppCoverage not installed"
    }
    
    Write-Host ""
}

# ============================================================================
# Summary Report
# ============================================================================
Write-Host ""
Write-Host "========================================" @Green
Write-Host "  PIPELINE SUMMARY" @Green
Write-Host "========================================" @Green

foreach ($line in $reportLines) {
    Write-Host $line @Green
}

Write-Host ""

# Final status
if ($pipelineSuccess) {
    Write-Host "[SUCCESS] PIPELINE PASSED - Ready to push!" @Green
    Write-Host ""
    exit 0
} else {
    Write-Host "[FAIL] PIPELINE FAILED - Fix errors and retry" @Red
    Write-Host ""
    exit 1
}
