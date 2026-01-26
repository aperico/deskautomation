<#
.SYNOPSIS
    Local development pipeline script for deskautomation project.

.DESCRIPTION
    This script provides common development tasks for building, testing, and analyzing the project.
    Run with -Help to see available commands.

.PARAMETER Command
    The command to execute. Valid values:
    - clean-build: Clean build directory and build the project
    - rebuild: Full rebuild (clean + configure + build)
    - test: Run all test cases
    - clean-test: Clean, build, and run all test cases
    - static-analysis: Run static analysis (cppcheck)
    - all: Run everything (rebuild + test + static analysis)

.PARAMETER Verbose
    Enable verbose output

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File .\run-pipeline.ps1 -Command clean-build
    powershell -ExecutionPolicy Bypass -File .\run-pipeline.ps1 -Command test
    powershell -ExecutionPolicy Bypass -File .\run-pipeline.ps1 -Command all

.NOTES
    Requires MSYS2 with MinGW-w64 toolchain and CMake installed.
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory=$false)]
    [ValidateSet('clean-build', 'rebuild', 'test', 'clean-test', 'static-analysis', 'all', 'help')]
    [string]$Command = 'help',
    
    [Parameter(Mandatory=$false)]
    [switch]$VerboseOutput
)

# Script configuration
$ErrorActionPreference = "Stop"
$script:WorkspaceRoot = Split-Path -Parent $PSScriptRoot
$script:BuildDir = Join-Path $script:WorkspaceRoot "build"
$script:SourceDir = Join-Path $script:WorkspaceRoot "src"
$script:ResultsDir = Join-Path $PSScriptRoot "results"
$script:MinGWPath = "C:/msys64/mingw64/bin"

# Create results directory if it doesn't exist
if (-not (Test-Path $script:ResultsDir)) {
    New-Item -ItemType Directory -Path $script:ResultsDir -Force | Out-Null
}

# Add MinGW to PATH
$env:PATH = "$script:MinGWPath;$env:PATH"

# Color output functions
function Global:Write-PipelineSuccess {
    param([string]$Message)
    Write-Host "[OK] $Message" -ForegroundColor Green
}

function Global:Write-PipelineInfo {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Cyan
}

function Global:Write-PipelineError {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

function Global:Write-PipelineHeader {
    param([string]$Message)
    Write-Host "`n========================================" -ForegroundColor Yellow
    Write-Host $Message -ForegroundColor Yellow
    Write-Host "========================================`n" -ForegroundColor Yellow
}

# Command functions
function Invoke-Clean {
    Write-PipelineHeader "CLEANING BUILD DIRECTORY"
    
    if (Test-Path $script:BuildDir) {
        Write-PipelineInfo "Removing build directory..."
        Remove-Item -Path $script:BuildDir -Recurse -Force -ErrorAction SilentlyContinue
        Write-PipelineSuccess "Build directory cleaned"
    } else {
        Write-PipelineInfo "Build directory does not exist, nothing to clean"
    }
    
    # Recreate build directory
    New-Item -ItemType Directory -Path $script:BuildDir -Force | Out-Null
    Write-PipelineSuccess "Build directory created"
    return $true
}

function Invoke-Configure {
    Write-PipelineHeader "CONFIGURING CMAKE"
    
    Set-Location $script:WorkspaceRoot
    Write-PipelineInfo "Running CMake configuration..."
    
    # Fixed filename without timestamp
    $configResultFile = Join-Path $script:ResultsDir "cmake_configure.txt"
    
    $ErrorActionPreference = "Continue"
    $configOutput = cmake -S . -B build 2>&1
    $exitCode = $LASTEXITCODE
    $ErrorActionPreference = "Stop"
    
    # Save configuration output to file with enhanced header
    $logHeader = @"
================================================================================
CMAKE CONFIGURATION LOG
================================================================================
Command: cmake -S . -B build
Timestamp: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Workspace: $script:WorkspaceRoot

This log contains the output from CMake configuration phase, including
compiler detection, dependency resolution, and build system generation.
================================================================================

"@
    $logHeader | Out-File -FilePath $configResultFile -Encoding UTF8
    $configOutput | Out-File -FilePath $configResultFile -Encoding UTF8 -Append
    
    # Check for warnings and display them
    $warnings = $configOutput | Select-String -Pattern "Warning|warning" -CaseSensitive:$false
    if ($warnings) {
        Write-Host "`nConfiguration Warnings:" -ForegroundColor Yellow
        $warnings | ForEach-Object { Write-Host "  $_" -ForegroundColor Yellow }
    }
    
    if ($exitCode -ne 0) {
        Write-PipelineError "CMake configuration failed"
        Write-Host $configOutput
        Write-PipelineInfo "Configuration log saved to: $configResultFile"
        return $false
    }
    
    Write-PipelineSuccess "CMake configuration completed"
    Write-PipelineInfo "Configuration log saved to: $configResultFile"
    return $true
}

function Invoke-Build {
    Write-PipelineHeader "BUILDING PROJECT"
    
    Set-Location $script:WorkspaceRoot
    Write-PipelineInfo "Building project (Release configuration)..."
    
    # Fixed filename without timestamp
    $buildResultFile = Join-Path $script:ResultsDir "build_log.txt"
    
    $ErrorActionPreference = "Continue"
    $buildOutput = cmake --build build --config Release 2>&1
    $exitCode = $LASTEXITCODE
    $ErrorActionPreference = "Stop"
    
    # Save build output to file with enhanced header
    $logHeader = @"
================================================================================
BUILD LOG
================================================================================
Command: cmake --build build --config Release
Timestamp: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Workspace: $script:WorkspaceRoot
Configuration: Release

This log contains the complete build output including compilation of all
source files, linking, and any compiler warnings or errors encountered.
================================================================================

"@
    $logHeader | Out-File -FilePath $buildResultFile -Encoding UTF8
    $buildOutput | Out-File -FilePath $buildResultFile -Encoding UTF8 -Append
    
    # Extract and display compiler warnings
    $warnings = $buildOutput | Select-String -Pattern "warning C[0-9]+:|warning:|[-Wno-]|note:" -CaseSensitive:$false
    if ($warnings) {
        Write-Host "`nCompiler Warnings:" -ForegroundColor Yellow
        $warnings | ForEach-Object { 
            $line = $_.ToString()
            if ($line -match "warning") {
                Write-Host "  $line" -ForegroundColor Yellow
            } else {
                Write-Host "  $line" -ForegroundColor Cyan
            }
        }
    }
    
    if ($exitCode -ne 0) {
        Write-PipelineError "Build failed"
        Write-Host "`nBuild Errors:" -ForegroundColor Red
        $buildOutput | ForEach-Object { Write-Host "  $_" }
        Write-PipelineInfo "Build log saved to: $buildResultFile"
        return $false
    }
    
    Write-PipelineSuccess "Build completed successfully"
    if (-not $warnings) {
        Write-PipelineSuccess "No compiler warnings detected"
    }
    Write-PipelineInfo "Build log saved to: $buildResultFile"
    return $true
}

function Invoke-Test {
    Write-PipelineHeader "RUNNING TESTS"
    
    Set-Location $script:WorkspaceRoot
    Write-PipelineInfo "Running all test cases with CTest..."
    
    # Fixed filename without timestamp
    $testResultFile = Join-Path $script:ResultsDir "test_results.txt"
    
    $ErrorActionPreference = "Continue"
    $testOutput = ctest --test-dir build -C Release --output-on-failure 2>&1
    $exitCode = $LASTEXITCODE
    $ErrorActionPreference = "Stop"
    
    # Save test output to file with enhanced header
    $logHeader = @"
================================================================================
TEST RESULTS
================================================================================
Command: ctest --test-dir build -C Release --output-on-failure
Timestamp: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Workspace: $script:WorkspaceRoot
Configuration: Release

This log contains the results of all test executions including unit tests,
component tests, and integration tests. Failed test details are included.
================================================================================

"@
    $logHeader | Out-File -FilePath $testResultFile -Encoding UTF8
    $testOutput | Out-File -FilePath $testResultFile -Encoding UTF8 -Append
    
    # Parse test summary
    $testSummary = $testOutput | Select-String -Pattern "tests passed|Test #|\d+% tests passed"
    if ($testSummary) {
        Write-Host "`nTest Summary:" -ForegroundColor Cyan
        $testSummary | ForEach-Object { Write-Host "  $_" -ForegroundColor Cyan }
    }
    
    if ($exitCode -ne 0) {
        Write-PipelineError "Tests failed"
        Write-Host "`nTest Output:" -ForegroundColor Red
        Write-Host $testOutput
        Write-PipelineInfo "Test results saved to: $testResultFile"
        return $false
    }
    
    Write-PipelineSuccess "All tests passed"
    Write-PipelineInfo "Test results saved to: $testResultFile"
    return $true
}

function Invoke-StaticAnalysis {
    Write-PipelineHeader "RUNNING STATIC ANALYSIS"
    
    # Check if cppcheck is available
    $cppcheckPath = Get-Command cppcheck -ErrorAction SilentlyContinue
    if (-not $cppcheckPath) {
        Write-PipelineError "cppcheck not found in PATH"
        Write-PipelineInfo "Install cppcheck via MSYS2: pacman -S mingw-w64-x86_64-cppcheck"
        return $false
    }
    
    Set-Location $script:WorkspaceRoot
    Write-PipelineInfo "Running comprehensive static analysis with cppcheck..."
    
    # Fixed filenames without timestamp
    $analysisResultFile = Join-Path $script:ResultsDir "static_analysis.txt"
    $analysisXmlFile = Join-Path $script:ResultsDir "static_analysis.xml"
    
    # Comprehensive cppcheck arguments for safety-critical code
    $cppcheckArgs = @(
        "--enable=all",
        "--std=c++17",
        "--inconclusive",
        "--inline-suppr",
        "--force",
        "--verbose",
        "-I$script:SourceDir",
        "--suppress=missingIncludeSystem",
        "--suppress=missingInclude",
        "--suppress=unusedFunction",
        "--suppress=unmatchedSuppression",
        "--suppress=checkersReport",
        "--error-exitcode=1",
        "--template=gcc",
        "--xml",
        "--xml-version=2",
        "--output-file=$analysisXmlFile",
        $script:SourceDir
    )
    
    Write-PipelineInfo "Analysis includes: errors, warnings, style, performance, portability checks"
    
    $analysisOutput = & cppcheck $cppcheckArgs 2>&1
    $exitCode = $LASTEXITCODE
    
    # Build the full command string for display
    $fullCommand = "cppcheck " + ($cppcheckArgs -join " ")
    
    # Save text analysis output to file with enhanced header
    $logHeader = @"
================================================================================
STATIC ANALYSIS REPORT
================================================================================
Command: $fullCommand
Timestamp: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Source Directory: $script:SourceDir
Analysis Level: Comprehensive

This report contains static code analysis results using cppcheck with all
checks enabled: errors, warnings, style, performance, and portability issues.
Results are saved in both text and XML formats for review and CI integration.
================================================================================

"@
    $logHeader | Out-File -FilePath $analysisResultFile -Encoding UTF8
    $analysisOutput | Out-File -FilePath $analysisResultFile -Encoding UTF8 -Append
    
    # Parse XML to count and display issues by severity
    if (Test-Path $analysisXmlFile) {
        [xml]$xmlContent = Get-Content $analysisXmlFile
        $errors = $xmlContent.results.errors.error
        
        if ($errors) {
            $errorCount = ($errors | Where-Object { $_.severity -eq "error" }).Count
            $warningCount = ($errors | Where-Object { $_.severity -eq "warning" }).Count
            $styleCount = ($errors | Where-Object { $_.severity -eq "style" }).Count
            $performanceCount = ($errors | Where-Object { $_.severity -eq "performance" }).Count
            $portabilityCount = ($errors | Where-Object { $_.severity -eq "portability" }).Count
            
            Write-Host "`nStatic Analysis Issues Found:" -ForegroundColor Yellow
            if ($errorCount -gt 0) { Write-Host "  Errors:       $errorCount" -ForegroundColor Red }
            if ($warningCount -gt 0) { Write-Host "  Warnings:     $warningCount" -ForegroundColor Magenta }
            if ($styleCount -gt 0) { Write-Host "  Style:        $styleCount" -ForegroundColor Cyan }
            if ($performanceCount -gt 0) { Write-Host "  Performance:  $performanceCount" -ForegroundColor Yellow }
            if ($portabilityCount -gt 0) { Write-Host "  Portability:  $portabilityCount" -ForegroundColor Yellow }
            
            # Display each issue in human-readable format
            Write-Host "`nIssue Details:" -ForegroundColor Yellow
            foreach ($error in $errors | Where-Object { $_.severity -ne "information" }) {
                $severityColor = switch ($error.severity) {
                    "error" { "Red" }
                    "warning" { "Magenta" }
                    "style" { "Cyan" }
                    default { "Yellow" }
                }
                Write-Host "`n  [$($error.severity.ToUpper())] $($error.id)" -ForegroundColor $severityColor
                Write-Host "  Message: $($error.msg)" -ForegroundColor White
                if ($error.location) {
                    $loc = $error.location | Select-Object -First 1
                    Write-Host "  File: $($loc.file)" -ForegroundColor Gray
                    Write-Host "  Line: $($loc.line), Column: $($loc.column)" -ForegroundColor Gray
                }
            }
            
            # Append human-readable summary to text file
            $summary = "`n`n" + "=" * 80 + "`nSUMMARY`n" + "=" * 80 + "`n"
            $summary += "Errors:       $errorCount`n"
            $summary += "Warnings:     $warningCount`n"
            $summary += "Style:        $styleCount`n"
            $summary += "Performance:  $performanceCount`n"
            $summary += "Portability:  $portabilityCount`n"
            $summary | Out-File -FilePath $analysisResultFile -Encoding UTF8 -Append
        }
    }
    
    if ($exitCode -ne 0) {
        Write-PipelineError "Static analysis found issues that need attention"
        Write-PipelineInfo "Static analysis results saved to:"
        Write-PipelineInfo "  - Text: $analysisResultFile"
        Write-PipelineInfo "  - XML:  $analysisXmlFile"
        return $false
    }
    
    Write-PipelineSuccess "Static analysis completed (no critical issues found)"
    Write-PipelineInfo "Static analysis results saved to:"
    Write-PipelineInfo "  - Text: $analysisResultFile"
    Write-PipelineInfo "  - XML:  $analysisXmlFile"
    return $true
}

function Show-Help {
    Write-Host @"

Local Development Pipeline Script
==================================

Usage:
    .\run-pipeline.ps1 -Command <command>

Available Commands:
    clean-build       Clean build directory and build the project
    rebuild           Full rebuild (clean + configure + build)
    test              Run all test cases
    clean-test        Clean, build, and run all test cases
    static-analysis   Run static analysis (cppcheck)
    all               Run everything (rebuild + test + static analysis)
    help              Show this help message

Examples:
    .\run-pipeline.ps1 -Command clean-build
    .\run-pipeline.ps1 -Command test
    .\run-pipeline.ps1 -Command clean-test
    .\run-pipeline.ps1 -Command all

Requirements:
    - MSYS2 with MinGW-w64 toolchain
    - CMake
    - cppcheck (for static analysis)

"@ -ForegroundColor White
}

# Main execution
function Main {
    $startTime = Get-Date
    
    Write-Host "`nLocal Development Pipeline" -ForegroundColor Magenta
    Write-Host "Command: $Command`n" -ForegroundColor Magenta
    
    try {
        $success = $true
        
        switch ($Command) {
            'clean-build' {
                if (-not (Invoke-Clean)) { $success = $false }
                if ($success -and -not (Invoke-Configure)) { $success = $false }
                if ($success -and -not (Invoke-Build)) { $success = $false }
            }
            'rebuild' {
                if (-not (Invoke-Clean)) { $success = $false }
                if ($success -and -not (Invoke-Configure)) { $success = $false }
                if ($success -and -not (Invoke-Build)) { $success = $false }
            }
            'test' {
                if (-not (Invoke-Test)) { $success = $false }
            }
            'clean-test' {
                if (-not (Invoke-Clean)) { $success = $false }
                if ($success -and -not (Invoke-Configure)) { $success = $false }
                if ($success -and -not (Invoke-Build)) { $success = $false }
                if ($success -and -not (Invoke-Test)) { $success = $false }
            }
            'static-analysis' {
                if (-not (Invoke-StaticAnalysis)) { $success = $false }
            }
            'all' {
                if (-not (Invoke-Clean)) { $success = $false }
                if ($success -and -not (Invoke-Configure)) { $success = $false }
                if ($success -and -not (Invoke-Build)) { $success = $false }
                if ($success -and -not (Invoke-Test)) { $success = $false }
                if ($success -and -not (Invoke-StaticAnalysis)) { $success = $false }
            }
            'help' {
                Show-Help
                exit 0
            }
            default {
                Show-Help
                exit 0
            }
        }
        
        if (-not $success) {
            Write-PipelineError "Pipeline failed"
            exit 1
        }
        
        $endTime = Get-Date
        $duration = $endTime - $startTime
        
        Write-Host "`n" -NoNewline
        Write-PipelineSuccess "Pipeline completed successfully in $($duration.TotalSeconds.ToString('F2')) seconds"
        
    } catch {
        Write-PipelineError "Pipeline failed: $_"
        exit 1
    }
}

# Run main function
Main
