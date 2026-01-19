# Windows Toolchain Setup Guide

This guide provides step-by-step instructions for setting up the development environment and building the Desk Automation project on Windows machines.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Toolchain Installation](#toolchain-installation)
- [Environment Configuration](#environment-configuration)
- [Building the Project](#building-the-project)
- [Running Tests](#running-tests)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

- **Windows 10/11** (64-bit)
- **Internet connection** for downloading packages
- **Administrator privileges** (for installation)

---

## Toolchain Installation

### Step 0: Install Cppcheck (Required for MISRA Checks)

Install Cppcheck for static analysis:

```powershell
# Install via Windows Package Manager
winget install --id Cppcheck.Cppcheck --accept-package-agreements --accept-source-agreements
```

**After installation, RESTART YOUR PC** to update PATH environment variable.

**Verify installation:**

```powershell
cppcheck --version
```

Expected output:
```
Cppcheck 2.19.0
```

If not recognized, see [Troubleshooting: Cppcheck not recognized](#troubleshooting) below.

### Step 1: Search for MSYS2 (Optional)

To verify MSYS2 is available via winget:

```powershell
winget search msys2
```

### Step 2: Install MSYS2

Install MSYS2 using Windows Package Manager (winget):

```powershell
winget install --id MSYS2.MSYS2 --silent --accept-package-agreements --accept-source-agreements
```

This will install MSYS2 to the default location: `C:\msys64`

### Step 3: Update Package Database

Update the MSYS2 package database:

```powershell
C:\msys64\usr\bin\bash.exe -lc "pacman -Sy --noconfirm"
```

### Step 4: Install Development Tools

Install the required toolchain components:

```powershell
C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-gtest mingw-w64-x86_64-make"
```

This installs:
- **g++ (GCC)** 15.2.0 - C++ compiler
- **CMake** 4.2.1 - Build system generator
- **Google Test** 1.17.0 - C++ testing framework
- **mingw32-make** 4.4.1 - Build automation tool
- All required dependencies

### Step 5: Verify Installation

Verify g++ is installed correctly:

```powershell
C:\msys64\mingw64\bin\g++.exe --version
```

Expected output:
```
g++.exe (Rev8, Built by MSYS2 project) 15.2.0
```

---

## Environment Configuration

### Add MSYS2 to PATH

Add the MSYS2 MinGW64 bin directory to your user PATH environment variable:

```powershell
$currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($currentPath -notlike "*C:\msys64\mingw64\bin*") {
    [Environment]::SetEnvironmentVariable("Path", "$currentPath;C:\msys64\mingw64\bin", "User")
    Write-Host "Added C:\msys64\mingw64\bin to user PATH"
} else {
    Write-Host "C:\msys64\mingw64\bin is already in PATH"
}
```

### Refresh PATH in Current Session

Reload the PATH environment variable in your current PowerShell session:

```powershell
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
```

### Verify Tools Are Accessible

Confirm all tools are accessible from any directory:

```powershell
g++ --version
cmake --version
mingw32-make --version
```

---

## Building the Project

### Method 1: Using Direct CMake Commands

#### Configure the Project

```powershell
cmake -S . -B build
```

#### Build the Project

```powershell
cmake --build build --config Release
```

### Method 2: Using CMake Presets

#### Configure with Preset

```powershell
cmake --preset ninja-msys
```

#### Build with Preset

```powershell
cmake --build --preset release
```

### Method 3: Using VS Code Tasks

The project includes predefined VS Code tasks for building:

1. Open the Command Palette: `Ctrl+Shift+P`
2. Run: `Tasks: Run Task`
3. Select one of the following:
   - **CMake: Clean** - Clean the build directory
   - **CMake: Configure** - Configure the project
   - **CMake: Build** - Build the project

---

## Running Tests

### Run All Tests

Execute all test cases:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

Or using CMake preset:

```powershell
ctest --preset all-tests
```

### Run Single Test

Execute a specific test case:

```powershell
ctest --test-dir build -C Release -R "^SmokeTest\.BasicTruth$" --output-on-failure
```

Replace `SmokeTest.BasicTruth` with your desired test name.

### Using VS Code Tasks

Run tests via VS Code:

1. Open the Command Palette: `Ctrl+Shift+P`
2. Run: `Tasks: Run Task`
3. Select:
   - **CTest: Run All** - Run all tests
   - **CTest: Run Single (SmokeTest.BasicTruth)** - Run specific test

---

## Troubleshooting

### Issue: "cppcheck is not recognized"

**Solution 1: Restart PowerShell**

```powershell
# Close current PowerShell and open a new one
exit

# Then try again
cppcheck --version
```

**Solution 2: Manually add Cppcheck to PATH**

If restart doesn't work, manually add Cppcheck to PATH:

```powershell
# Find Cppcheck installation location
$cppcheckPath = (Get-Command cppcheck -ErrorAction SilentlyContinue).Source
if ($null -eq $cppcheckPath) {
    # Default installation location
    $cppcheckBin = "C:\Program Files\Cppcheck"
    if (Test-Path $cppcheckBin) {
        Write-Host "Adding $cppcheckBin to PATH"
        $currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
        if ($currentPath -notlike "*$cppcheckBin*") {
            [Environment]::SetEnvironmentVariable("Path", "$currentPath;$cppcheckBin", "User")
            $env:Path += ";$cppcheckBin"
        }
    } else {
        Write-Host "Cppcheck not found in default location"
    }
}

# Verify
cppcheck --version
```

**Solution 3: Manual Download (If winget fails)**

1. Download from: https://github.com/danmar/cppcheck/releases
2. Extract to: `C:\Program Files\Cppcheck`
3. Add to PATH (see Solution 2 above)

### Issue: "g++ is not recognized"

**Solution:** Ensure `C:\msys64\mingw64\bin` is in your PATH and restart your terminal.

### Issue: CMake configuration fails

**Solution:** 
1. Clean the build directory:
   ```powershell
   Remove-Item -Path ".\build" -Recurse -Force -ErrorAction SilentlyContinue
   ```
2. Reconfigure the project:
   ```powershell
   cmake -S . -B build
   ```

### Issue: Tests fail to run

**Solution:** 
1. Ensure the project is built successfully
2. Verify Google Test is installed:
   ```powershell
   C:\msys64\usr\bin\bash.exe -lc "pacman -Q mingw-w64-x86_64-gtest"
   ```

### Issue: PATH not updating in new terminals

**Solution:** Close and reopen all terminals/applications, or log out and log back into Windows.

---

## Additional Resources

- [MSYS2 Official Documentation](https://www.msys2.org/)
- [CMake Documentation](https://cmake.org/documentation/)
- [Google Test Documentation](https://google.github.io/googletest/)

---

## CI/CD Pipeline - Local Execution (Windows)

This section explains how to run the same pipeline commands locally on your Windows machine for testing before pushing to GitHub.

### Prerequisites

Install the following tools locally:

```powershell
# Install Cppcheck for static analysis
winget install --id Cppcheck.Cppcheck --silent --accept-package-agreements

# Install optional tools
winget install --id LLVM.LLVM --silent --accept-package-agreements
```

### Command Reference

#### 1. Static Analysis (MISRA-C 2012 Checks)

```powershell
# Run MISRA-C 2012 checks on source code
cppcheck --enable=all --inconclusive --std=c++11 --suppress=missingIncludeSystem `
  --addon=misra --addon=cert `
  --xml --xml-version=2 `
  --output-file=cppcheck-report.xml `
  source

# View human-readable summary
cppcheck --enable=all --std=c++11 --suppress=missingIncludeSystem `
  --addon=misra --addon=cert `
  source 2>&1 | Tee-Object -FilePath cppcheck-summary.txt
```

**Output Files:**
- `cppcheck-report.xml` - Detailed XML report for tools
- `cppcheck-summary.txt` - Human-readable violations summary

#### 2. Clean Build & Configure

```powershell
# Remove old build directory
Remove-Item -Path ".\build" -Recurse -Force -ErrorAction SilentlyContinue

# Configure project with CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

#### 3. Standard Build & Test (Release)

```powershell
# Build in Release mode
cmake --build build --config Release

# Run all tests with output
ctest --test-dir build -C Release --output-on-failure
```

#### 4. Build with Address Sanitizer (Clang)

**Note:** Requires Clang (install via LLVM package)

```powershell
# Configure with Clang and address sanitizer
cmake -S . -B build-asan `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER="clang" `
  -DCMAKE_CXX_COMPILER="clang++" `
  -DCMAKE_CXX_FLAGS="-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -g"

# Build
cmake --build build-asan --config Debug

# Run tests with AddressSanitizer
$env:ASAN_OPTIONS="verbosity=2:halt_on_error=1"
$env:UBSAN_OPTIONS="verbosity=2:halt_on_error=1"
ctest --test-dir build-asan -C Debug --output-on-failure 2>&1 | Tee-Object -FilePath asan-ubsan-results.txt
```

**Note:** If Clang not available, alternative is to use WSL (see below).

#### 5. Code Coverage (Debug Build)

```powershell
# Configure with coverage flags (requires Debug mode)
cmake -S . -B build-coverage `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_CXX_FLAGS="--coverage" `
  -DCMAKE_C_FLAGS="--coverage"

# Build
cmake --build build-coverage --config Debug

# Run tests (generates .gcda files)
ctest --test-dir build-coverage -C Debug --output-on-failure

# Generate coverage report (requires OpenCppCoverage or similar)
# Using OpenCppCoverage (install via: winget install OpenCppCoverage.OpenCppCoverage)
OpenCppCoverage.exe --modules build-coverage `
  --sources source `
  -- ctest --test-dir build-coverage -C Debug
```

#### 6. Compile & Run Fuzzing Harness

```powershell
# Compile fuzzing harness
$harness = @'
#include <iostream>
#include <random>
#include <chrono>
extern "C" {
  #include "source/arduino/DeskController.h"
  #include "source/arduino/HAL.h"
}

int main() {
  std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<> dis(0, 3);
  
  DeskAppInputs_t inputs = {};
  DeskAppOutputs_t outputs = {};
  DeskApp_task_init(&inputs, &outputs);
  
  std::cout << "=== Fuzzing DeskApp_task with 1000 random inputs ===" << std::endl;
  for (int i = 0; i < 1000; i++) {
    inputs.switch_state = dis(gen);
    inputs.current_sense = dis(gen) * 25;
    
    DeskAppTask_Return_t ret = DeskApp_task(&inputs, &outputs);
    
    if (ret != APP_TASK_SUCCESS) {
      std::cout << "Iteration " << i << ": Unexpected return code " << ret << std::endl;
    }
    if (outputs.motor_pwm > 255) {
      std::cout << "FUZZ CRASH: PWM out of range at iteration " << i << std::endl;
      return 1;
    }
  }
  std::cout << "Fuzzing completed successfully - 1000 iterations passed" << std::endl;
  return 0;
}
'@

$harness | Out-File -FilePath fuzzing-harness.cpp -Encoding UTF8

# Compile
g++ -DDESK_CONTROLLER_ENABLE_TEST_INTERFACE -std=c++17 `
  -I./source/arduino -I./tests `
  fuzzing-harness.cpp source/arduino/DeskController.cpp `
  -o fuzzing-harness.exe

# Run fuzzing
./fuzzing-harness.exe | Tee-Object -FilePath fuzzing-results.txt
```

#### 7. Comprehensive Quality Report

```powershell
# Run all checks and generate a summary

Write-Host "=== COMPREHENSIVE LOCAL QA REPORT ===" -ForegroundColor Green
Write-Host ""

# 1. Static Analysis
Write-Host "1. Running MISRA-C checks..." -ForegroundColor Yellow
cppcheck --enable=all --std=c++11 --suppress=missingIncludeSystem `
  --addon=misra --addon=cert `
  source 2>&1 | Out-File -FilePath cppcheck-local.txt

$misraCount = (Get-Content cppcheck-local.txt | Measure-Object -Line).Lines
Write-Host "   ✓ MISRA check complete ($misraCount issues)" -ForegroundColor Green

# 2. Build & Test
Write-Host "2. Building project..." -ForegroundColor Yellow
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release | Out-Null
cmake --build build --config Release | Out-Null
Write-Host "   ✓ Build successful" -ForegroundColor Green

# 3. Run Tests
Write-Host "3. Running tests..." -ForegroundColor Yellow
$testOutput = ctest --test-dir build -C Release --output-on-failure 2>&1
$testOutput | Tee-Object -FilePath test-results-local.txt | Select-Object -Last 5
Write-Host "   ✓ Tests complete" -ForegroundColor Green

# 4. Summary
Write-Host ""
Write-Host "=== SUMMARY ===" -ForegroundColor Green
Write-Host "✓ MISRA-C checks: $(Get-Item cppcheck-local.txt | Select-Object -ExpandProperty Length) bytes"
Write-Host "✓ Test results: See test-results-local.txt"
Write-Host "✓ All reports saved in current directory"
Write-Host ""
```

### Running the Complete Pipeline Locally

Create a batch script to run all checks:

```powershell
# Save as: run-pipeline.ps1

param(
    [switch]$Full = $false,     # Run full pipeline (including ASan)
    [switch]$Coverage = $false  # Generate coverage
)

Write-Host "=== DESK AUTOMATION LOCAL CI PIPELINE ===" -ForegroundColor Cyan
Write-Host "Date: $(Get-Date)" -ForegroundColor Gray
Write-Host ""

# Static Analysis
Write-Host "Step 1: Static Analysis (MISRA-C 2012)" -ForegroundColor Yellow
cppcheck --enable=all --std=c++11 --suppress=missingIncludeSystem `
  --addon=misra --addon=cert source 2>&1 | Tee-Object -FilePath cppcheck-results.txt

# Clean & Configure
Write-Host "`nStep 2: Configure Build" -ForegroundColor Yellow
Remove-Item -Path ".\build" -Recurse -Force -ErrorAction SilentlyContinue
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
Write-Host "Step 3: Build Project" -ForegroundColor Yellow
cmake --build build --config Release

# Tests
Write-Host "Step 4: Run Tests" -ForegroundColor Yellow
ctest --test-dir build -C Release --output-on-failure 2>&1 | Tee-Object -FilePath test-results.txt

if ($Full) {
    Write-Host "`nStep 5: AddressSanitizer (Requires Clang)" -ForegroundColor Yellow
    # AddressSanitizer commands here
}

if ($Coverage) {
    Write-Host "`nStep 6: Code Coverage" -ForegroundColor Yellow
    Write-Host "Note: Requires OpenCppCoverage or gcov" -ForegroundColor Gray
}

Write-Host "`n=== PIPELINE COMPLETE ===" -ForegroundColor Green
Write-Host "Results saved to: cppcheck-results.txt, test-results.txt" -ForegroundColor Gray
```

**Execute the pipeline:**

```powershell
# Run standard checks
./run-pipeline.ps1

# Run with full dynamic analysis (requires Clang)
./run-pipeline.ps1 -Full

# Run with coverage (requires OpenCppCoverage)
./run-pipeline.ps1 -Coverage
```

### Using WSL (Windows Subsystem for Linux) for Full Pipeline

For complete CI/CD pipeline parity with GitHub (including Valgrind, full ASan support):

```powershell
# 1. Install WSL2 with Ubuntu
wsl --install -d Ubuntu

# 2. Inside WSL, install tools
wsl bash -c "sudo apt-get update && sudo apt-get install -y cmake g++ gcc lcov cppcheck valgrind libgtest-dev"

# 3. Navigate to project in WSL
wsl -d Ubuntu -e bash -c "cd /mnt/c/path/to/project && bash"

# 4. Run full pipeline inside WSL
cmake -S . -B build-wsl
cmake --build build-wsl --config Release
ctest --test-dir build-wsl --output-on-failure
```

### Summary Table: Local Execution

| Feature | Windows Native | WSL | Notes |
|---------|---|---|---|
| MISRA-C Checks | ✅ | ✅ | Install Cppcheck |
| Build & Test | ✅ | ✅ | Using MSYS2/MinGW |
| AddressSanitizer | ⚠️ | ✅ | Requires Clang/LLVM |
| Valgrind | ❌ | ✅ | Linux-only tool |
| Fuzzing | ✅ | ✅ | Custom harness |
| Coverage | ⚠️ | ✅ | Requires OpenCppCoverage |

**Recommendation:** Use Windows native for quick checks, WSL for complete CI parity.

---

