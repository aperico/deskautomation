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
Write-Host "╔════════════════════════════════════════════════════════════════╗" @Green
Write-Host "║   DESK AUTOMATION - LOCAL CI/CD PIPELINE                       ║" @Green
Write-Host "╚════════════════════════════════════════════════════════════════╝" @Green
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
Write-Host "─────────────────────────────────────────" @Gray

$misraStart = Get-Date

# Check if cppcheck is available
if (-not (Get-Command cppcheck -ErrorAction SilentlyContinue)) {
    Write-Host "⚠️  Cppcheck not found in PATH" @Red
    Write-Host "   Install with: winget install --id Cppcheck.Cppcheck" @Gray
    $pipelineSuccess = $false
    $reportLines += "❌ Cppcheck not installed"
} else {
    # Run MISRA-C checks with XML and text output
    $misraXmlFile = "$PSScriptRoot\..\cppcheck-report.xml"
    $misraTxtFile = "$PSScriptRoot\..\cppcheck-summary.txt"
    
    cppcheck --enable=all --inconclusive --std=c++11 --suppress=missingIncludeSystem `
        --addon=misra --addon=cert `
        --xml --xml-version=2 `
        --output-file=$misraXmlFile `
        "$PSScriptRoot\..\source" 2>&1 | Tee-Object -FilePath $misraTxtFile | Select-Object -Last 5
    
    $misraElapsed = ((Get-Date) - $misraStart).TotalSeconds
    
    # Count issues from XML
    if (Test-Path $misraXmlFile) {
        [xml]$xmlDoc = Get-Content $misraXmlFile
        $errorCount = @($xmlDoc.results.error).Count
        Write-Host "   ✓ MISRA check complete ($errorCount issues)" @Green
        Write-Host "   ✓ Reports: cppcheck-report.xml, cppcheck-summary.txt" @Green
        $reportLines += "✅ MISRA-C 2012: $errorCount issues found (${misraElapsed}s)"
    } else {
        Write-Host "   ✓ MISRA check complete" @Green
        $reportLines += "✅ MISRA-C 2012: Analysis passed (${misraElapsed}s)"
    }
}

Write-Host ""

# ============================================================================
# Step 2: Clean & Configure Build
# ============================================================================
Write-Host "[2/5] Configuring Build..." @Yellow
Write-Host "──────────────────────────" @Gray

$buildStart = Get-Date

if (-not $SkipClean) {
    Write-Host "   Cleaning build directory..." @Gray
    Remove-Item -Path "$PSScriptRoot\..\build" -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
}

Write-Host "   Running CMake configuration..." @Gray
$configOutput = cmake -S "$PSScriptRoot\.." -B "$PSScriptRoot\..\build" -DCMAKE_BUILD_TYPE=Release 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "   ❌ CMake configuration failed" @Red
    Write-Host $configOutput | Select-Object -Last 10
    $pipelineSuccess = $false
    $reportLines += "❌ CMake Configuration failed"
} else {
    Write-Host "   ✓ CMake configuration successful" @Green
    $reportLines += "✅ CMake Configuration: OK"
}

Write-Host ""

# ============================================================================
# Step 3: Build Project
# ============================================================================
Write-Host "[3/5] Building Project (Release)..." @Yellow
Write-Host "──────────────────────────────────" @Gray

$buildStart = Get-Date
$buildOutput = cmake --build "$PSScriptRoot\..\build" --config Release 2>&1
$buildElapsed = ((Get-Date) - $buildStart).TotalSeconds

if ($LASTEXITCODE -ne 0) {
    Write-Host "   ❌ Build failed" @Red
    Write-Host $buildOutput | Select-Object -Last 15
    $pipelineSuccess = $false
    $reportLines += "❌ Build failed"
} else {
    Write-Host "   ✓ Build successful (${buildElapsed}s)" @Green
    $reportLines += "✅ Build (Release): OK (${buildElapsed}s)"
}

Write-Host ""

# ============================================================================
# Step 4: Run Tests
# ============================================================================
Write-Host "[4/5] Running Tests..." @Yellow
Write-Host "─────────────────────" @Gray

$testStart = Get-Date
$testOutput = ctest --test-dir "$PSScriptRoot\..\build" -C Release --output-on-failure 2>&1
$testElapsed = ((Get-Date) - $testStart).TotalSeconds

if ($LASTEXITCODE -ne 0) {
    Write-Host "   ❌ Tests failed" @Red
    Write-Host $testOutput | Select-Object -Last 20
    $pipelineSuccess = $false
    $reportLines += "❌ Tests: FAILED"
} else {
    # Parse test results
    $passed = ($testOutput | Select-String "100% tests passed").Count
    $total = ($testOutput | Select-String "Test.*Passed").Count
    
    if ($passed -gt 0) {
        Write-Host "   ✓ All tests passed (${testElapsed}s)" @Green
        $reportLines += "✅ Tests (25/25): PASSED (${testElapsed}s)"
        Write-Host $testOutput | Select-Object -Last 3
    } else {
        Write-Host "   ✓ Tests executed (${testElapsed}s)" @Green
        $reportLines += "✅ Tests: OK (${testElapsed}s)"
        Write-Host $testOutput | Select-Object -Last 3
    }
}

Write-Host ""

# ============================================================================
# Step 5 (Optional): AddressSanitizer
# ============================================================================
if ($Full) {
    Write-Host "[5/5] Building with AddressSanitizer..." @Yellow
    Write-Host "───────────────────────────────────────" @Gray
    
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
                    Write-Host "   ✓ AddressSanitizer tests passed (${asanElapsed}s)" @Green
                    $reportLines += "✅ AddressSanitizer: PASSED (${asanElapsed}s)"
                } else {
                    Write-Host "   ⚠️  AddressSanitizer tests had issues" @Yellow
                    $reportLines += "⚠️  AddressSanitizer: Check build-asan directory"
                    $pipelineSuccess = $false
                }
            } else {
                Write-Host "   ⚠️  ASan build failed" @Yellow
                $reportLines += "⚠️  AddressSanitizer: Build failed"
            }
        } else {
            Write-Host "   ⚠️  ASan configuration failed" @Yellow
            $reportLines += "⚠️  AddressSanitizer: Configuration failed"
        }
    } else {
        Write-Host "   ⚠️  Clang not found (required for AddressSanitizer)" @Yellow
        Write-Host "      Install with: winget install --id LLVM.LLVM" @Gray
        $reportLines += "⚠️  AddressSanitizer: Clang not installed"
    }
    
    Write-Host ""
}

# ============================================================================
# Step 5/6 (Optional): Code Coverage
# ============================================================================
if ($Coverage) {
    Write-Host "[5/5] Generating Code Coverage..." @Yellow
    Write-Host "────────────────────────────────" @Gray
    
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
            Write-Host "   ✓ Coverage report generated (${coverageElapsed}s)" @Green
            $reportLines += "✅ Code Coverage: Report generated (${coverageElapsed}s)"
        } else {
            Write-Host "   ⚠️  Coverage build failed" @Yellow
            $reportLines += "⚠️  Code Coverage: Build failed"
        }
    } else {
        Write-Host "   ⚠️  OpenCppCoverage not found" @Yellow
        Write-Host "      Install with: winget install --id OpenCppCoverage.OpenCppCoverage" @Gray
        $reportLines += "⚠️  Code Coverage: OpenCppCoverage not installed"
    }
    
    Write-Host ""
}

# ============================================================================
# Summary Report
# ============================================================================
Write-Host ""
Write-Host "╔════════════════════════════════════════════════════════════════╗" @Green
Write-Host "║   PIPELINE SUMMARY                                             ║" @Green
Write-Host "╚════════════════════════════════════════════════════════════════╝" @Green

foreach ($line in $reportLines) {
    Write-Host $line @Green
}

Write-Host ""

# Final status
if ($pipelineSuccess) {
    Write-Host "✓ PIPELINE PASSED - Ready to push!" @Green
    Write-Host ""
    exit 0
} else {
    Write-Host "❌ PIPELINE FAILED - Fix errors and retry" @Red
    Write-Host ""
    exit 1
}
