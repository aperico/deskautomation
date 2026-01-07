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

## Notes

- **Installation Location:** All tools are installed under `C:\msys64\mingw64\bin\`
- **Default Build Location:** `build/` directory (git-ignored)
- **Configuration Files:** See [CMakeLists.txt](../CMakeLists.txt) and [CMakePresets.json](../CMakePresets.json)
- **For new terminal sessions:** PATH will be automatically available after the initial setup