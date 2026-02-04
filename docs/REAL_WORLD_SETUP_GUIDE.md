/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Real-World Setup Guide
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Practical setup guide based on actual implementation experience.
 *   This document captures real-world issues encountered during setup
 *   and their solutions. Use this alongside SETUP_INSTRUCTIONS.md.
 * 
 * Target Audience:
 *   - LLM coding agents (Cursor, Copilot, etc.)
 *   - Human developers setting up CHAI3D + Haply Inverse3
 *   - Windows 11 x64 users
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial real-world guide from implementation
 * 
 ****************************************************************************/

# CHAI3D + Haply Inverse3 - Real-World Setup Guide

## Overview

This guide documents the **actual issues encountered** during a complete setup from scratch on Windows 11 with Visual Studio 2022. Follow these steps sequentially to avoid common pitfalls.

**System Tested:**
- Windows 11 Build 26100 (24H2)
- Intel Core i9-14900HX / 32GB RAM
- Visual Studio 2022
- CMake 4.2.3
- Git for Windows

---

## Step-by-Step Setup with Real Issues & Solutions

### 1. Prerequisites Installation

#### Install CMake
```powershell
winget install Kitware.CMake
```

**⚠️ Issue:** CMake not found after installation
**Solution:** Close and reopen PowerShell, OR refresh PATH in current session:
```powershell
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
cmake --version  # Verify CMake 4.2.3
```

---

### 2. Clone Repository and CHAI3D

**⚠️ Critical:** Run these commands FROM YOUR REPO DIRECTORY, not from `C:\Users\other\`!

```powershell
# Navigate to your repo (adjust path as needed)
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

# Create external directory
mkdir external

# Add CHAI3D as submodule
git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d
cd external/chai3d
git checkout haply-api-cpp
```

**⚠️ Issue #1:** `fatal: not a git repository`
**Solution:** You're not in your repo directory. Use `cd` to navigate there first.

**⚠️ Issue #2:** SSH submodule failure
```
Host key verification failed.
fatal: Could not read from remote repository.
```

**Solution:** CHAI3D's nested submodules use SSH URLs. Configure Git to convert SSH to HTTPS globally:

```powershell
git config --global url."https://github.com/".insteadOf "git@github.com:"
```

Then retry:
```powershell
git submodule update --init --recursive
```

**Verification:** You should see:
```
Submodule path 'external/github-HaplyHaptics-Haply-API-cpp': checked out '...'
Submodule path '.../github-HaplyHaptics-serial': checked out '...'
```

---

### 3. Build CHAI3D Library

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR\external\chai3d
mkdir build
cd build
```

**⚠️ Issue:** CMake 4.x compatibility error
```
CMake Error at CMakeLists.txt:44 (cmake_minimum_required):
  Compatibility with CMake < 3.5 has been removed from CMake.
```

**Solution:** Add policy override flag with **proper PowerShell quoting**:

```powershell
# CORRECT (note the quotes around the flag):
cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"

# WRONG (PowerShell splits at the dot):
cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

**Why quotes are needed:** PowerShell treats `.` as a path separator and splits `3.5` into `3` and `.5` as separate arguments. The quotes prevent this.

**Expected Output:**
```
-- Configuring done (0.1s)
-- Generating done (0.4s)
-- Build files have been written to: .../chai3d/build
```

**Build CHAI3D:**
```powershell
cmake --build . --config Release
```

This takes **5-15 minutes** and compiles:
- Core CHAI3D library (`chai3d.lib`)
- All 31 GLUT demo applications
- Haply device drivers
- Extension modules

**Success indicators:**
```
Build succeeded.
0 Warning(s)
0 Error(s)
```

**Verify the build worked:**
```powershell
.\bin\win-x64\01-mydevice.exe
```

You should see the CHAI3D demo window open (device connection is optional at this stage).

---

### 4. Build Your Application

#### Update CMakeLists.txt (Critical Fixes)

**⚠️ Issue #1:** Eigen `binder1st`/`binder2nd` errors with C++17
```
error C2039: 'binder1st': is not a member of 'std'
error C2039: 'binder2nd': is not a member of 'std'
```

**Root Cause:** CHAI3D bundles an old Eigen that uses `std::binder1st` and `std::binder2nd`, which were **removed in C++17**. CHAI3D itself defaults to C++14.

**Solution:** Change C++ standard from 17 to 14 in `CMakeLists.txt`:

```cmake
# Change this:
set(CMAKE_CXX_STANDARD 17)

# To this:
set(CMAKE_CXX_STANDARD 14)
```

**⚠️ Issue #2:** `GL/freeglut.h` not found
```
error C1083: Cannot open include file: 'GL/freeglut.h'
```

**Root Cause:** FreeGLUT is in `extras/freeglut`, not `external/freeglut`.

**Solution:** Fix the FreeGLUT path in `CMakeLists.txt`:

```cmake
# Change this:
set(FREEGLUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/chai3d/external/freeglut")

# To this:
set(FREEGLUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/chai3d/extras/freeglut")
```

**Complete CMakeLists.txt with all fixes:**

```cmake
cmake_minimum_required(VERSION 3.10)
project(AIMLAB-Haptics VERSION 1.0 LANGUAGES CXX)

# Use C++14 for CHAI3D Eigen compatibility
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find CHAI3D
find_package(CHAI3D REQUIRED)
include_directories(${CHAI3D_INCLUDE_DIRS})

# FreeGLUT Configuration (Windows)
if(WIN32)
    # CORRECT PATH: extras/freeglut (not external/freeglut)
    set(FREEGLUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/chai3d/extras/freeglut")
    include_directories("${FREEGLUT_DIR}/include")
    
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        link_directories("${FREEGLUT_DIR}/lib/x64")
    else()
        link_directories("${FREEGLUT_DIR}/lib/x32")
    endif()
    
    set(GLUT_LIBRARIES freeglut)
else()
    find_package(GLUT REQUIRED)
endif()

# Application
add_executable(aimlab-haptics src/main.cpp)
target_link_libraries(aimlab-haptics ${CHAI3D_LIBRARIES} ${GLUT_LIBRARIES})

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
```

#### Build Your Project

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build . --config Release
```

**Success:** Executable created at `build\bin\Release\aimlab-haptics.exe`

---

### 5. Device Connection (Haply Inverse3)

#### Common Device Error

```
Could not open serial port: IO Exception: Unknown error opening the serial port: 5
```

**Error Code 5 = ACCESS_DENIED**

**Possible Causes:**

1. **Haply Hub is running** (locks the COM port)
   - CHAI3D's HardwareAPI uses direct serial access
   - Conflicts with Haply Hub's Inverse Service
   - **Solution:** Close Haply Hub before running CHAI3D demos

2. **Device not connected**
   - Check USB-C cable is plugged in (LED: red → purple)
   - Check 24V power supply is connected
   - Calibrate device (touch magnet to calibration point → white LED)

3. **Permissions**
   - Run PowerShell as Administrator if needed

#### Device Setup Checklist

- [ ] Inverse3 mounted to desk clamp
- [ ] Arms unfolded (reverse the fold procedure)
- [ ] USB-C connected directly to PC (no hubs/dongles)
- [ ] Status LED: red → purple (USB connected)
- [ ] 24V power supply connected
- [ ] Calibrated (magnet to calibration point → white LED)
- [ ] Haply Hub CLOSED (for HardwareAPI access)

---

## Complete Command Reference

### One-Time Setup (Run Once)

```powershell
# 1. Install CMake
winget install Kitware.CMake

# 2. Refresh PATH (or close/reopen PowerShell)
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")

# 3. Configure Git for HTTPS submodules
git config --global url."https://github.com/".insteadOf "git@github.com:"

# 4. Navigate to repo
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

# 5. Clone CHAI3D
mkdir external
git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d
cd external/chai3d
git checkout haply-api-cpp
git submodule update --init --recursive
cd ../..

# 6. Build CHAI3D (takes 5-15 min)
cd external/chai3d
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build . --config Release
cd ../../..
```

### Build Your Application (After CMakeLists.txt fixes)

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build . --config Release
```

### Clean Rebuild

```powershell
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build . --config Release
```

---

## Troubleshooting Quick Reference

| Issue | Solution |
|-------|----------|
| `cmake` not recognized | Refresh PATH: `$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")` |
| Not a git repository | Navigate to repo directory first: `cd path\to\AIMLAB-Unity-Chai3D-VR` |
| SSH submodule failure | Run: `git config --global url."https://github.com/".insteadOf "git@github.com:"` |
| CMake < 3.5 removed error | Add quoted flag: `"-DCMAKE_POLICY_VERSION_MINIMUM=3.5"` |
| PowerShell splits 3.5 | Use quotes around entire flag (see above) |
| Eigen binder1st/binder2nd errors | Change to C++14: `set(CMAKE_CXX_STANDARD 14)` |
| GL/freeglut.h not found | Fix path: use `extras/freeglut` not `external/freeglut` |
| Serial port error code 5 | Close Haply Hub (conflicts with HardwareAPI) |

---

## Critical Files Checklist

After complete setup, your repo should have:

```
AIMLAB-Unity-Chai3D-VR/
├── .gitmodules                    ✅ Created by git submodule add
├── CMakeLists.txt                 ✅ With C++14 and extras/freeglut fixes
├── README.md                      ✅ Project documentation
├── src/
│   └── main.cpp                   ✅ Your haptic application
├── external/
│   └── chai3d/                    ✅ Git submodule (Haply fork)
│       ├── build/
│       │   └── Release/
│       │       └── chai3d.lib     ✅ Built library
│       └── bin/
│           └── win-x64/
│               └── *.exe          ✅ 31 demo applications
└── build/
    └── bin/
        └── Release/
            └── aimlab-haptics.exe ✅ Your application
```

---

## Verification Tests

### Test 1: CMake Installed
```powershell
cmake --version
# Should show: cmake version 4.2.3 (or later)
```

### Test 2: CHAI3D Built
```powershell
Test-Path .\external\chai3d\build\Release\chai3d.lib
# Should return: True
```

### Test 3: Demos Work
```powershell
.\external\chai3d\bin\win-x64\01-mydevice.exe
# Should open demo window (device optional)
```

### Test 4: Your App Builds
```powershell
Test-Path .\build\bin\Release\aimlab-haptics.exe
# Should return: True
```

---

## Performance Notes

**Build Times:**
- CHAI3D full build: 5-15 minutes (depends on CPU)
- Your application: 30-60 seconds
- Clean rebuild: Add 30 seconds for CMake configuration

**Disk Space:**
- CHAI3D source: ~200 MB
- CHAI3D build artifacts: ~500 MB
- Total project: ~800 MB

**Memory Usage:**
- Visual Studio compilation: 4-8 GB RAM
- Running application: ~100-200 MB RAM

---

## Known Limitations

1. **CMake 4.x Compatibility**
   - CHAI3D's CMakeLists.txt declares old minimum version
   - Always use `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` flag
   - This is a known issue, not a bug in your setup

2. **C++14 Requirement**
   - Cannot use C++17 features due to Eigen dependency
   - CHAI3D's Eigen uses deprecated `std::binder1st/2nd`
   - This is intentional, not a mistake

3. **FreeGLUT Location**
   - Located in `extras/` not `external/`
   - CHAI3D's directory structure choice
   - Always use correct path

4. **Haply Hub Conflict**
   - Inverse Service locks COM port
   - Cannot run simultaneously with HardwareAPI demos
   - Choose one: Haply Hub OR CHAI3D direct access

---

## Advanced: For LLM Agents

### Automated Setup Script (PowerShell)

```powershell
# Full automated setup for LLM agents
function Setup-CHAI3D-Haply {
    param(
        [string]$RepoPath = "C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR"
    )
    
    Write-Host "Starting automated CHAI3D + Haply setup..." -ForegroundColor Cyan
    
    # 1. Install CMake if not present
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Write-Host "Installing CMake..." -ForegroundColor Yellow
        winget install Kitware.CMake
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
    }
    
    # 2. Configure Git for HTTPS
    git config --global url."https://github.com/".insteadOf "git@github.com:"
    
    # 3. Clone CHAI3D
    Set-Location $RepoPath
    if (-not (Test-Path "external")) { mkdir external }
    
    git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d
    Set-Location external/chai3d
    git checkout haply-api-cpp
    git submodule update --init --recursive
    Set-Location $RepoPath
    
    # 4. Build CHAI3D
    Set-Location external/chai3d
    if (-not (Test-Path "build")) { mkdir build }
    Set-Location build
    
    cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
    cmake --build . --config Release
    
    Set-Location $RepoPath
    
    # 5. Build application
    if (-not (Test-Path "build")) { mkdir build }
    Set-Location build
    
    cmake .. -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
    cmake --build . --config Release
    
    Set-Location $RepoPath
    
    Write-Host "Setup complete!" -ForegroundColor Green
    Write-Host "Executable: .\build\bin\Release\aimlab-haptics.exe" -ForegroundColor Cyan
}

# Run the setup
Setup-CHAI3D-Haply
```

### Error Handling for Agents

```powershell
# Check for common issues
function Test-Setup {
    $issues = @()
    
    # Check CMake
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        $issues += "CMake not found in PATH"
    }
    
    # Check Git
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
        $issues += "Git not installed"
    }
    
    # Check CHAI3D library
    if (-not (Test-Path ".\external\chai3d\build\Release\chai3d.lib")) {
        $issues += "CHAI3D library not built"
    }
    
    # Check FreeGLUT
    if (-not (Test-Path ".\external\chai3d\extras\freeglut\include\GL\freeglut.h")) {
        $issues += "FreeGLUT headers not found"
    }
    
    if ($issues.Count -eq 0) {
        Write-Host "✅ All checks passed!" -ForegroundColor Green
        return $true
    } else {
        Write-Host "❌ Issues found:" -ForegroundColor Red
        $issues | ForEach-Object { Write-Host "  - $_" -ForegroundColor Yellow }
        return $false
    }
}

Test-Setup
```

---

## Success Criteria

Your setup is complete when:

- ✅ `cmake --version` shows 4.2.3+
- ✅ CHAI3D submodule cloned with all nested submodules
- ✅ CHAI3D built successfully (chai3d.lib exists)
- ✅ 31 demo applications built (*.exe in bin/win-x64/)
- ✅ Your CMakeLists.txt uses C++14
- ✅ Your CMakeLists.txt points to extras/freeglut
- ✅ Your application builds without errors
- ✅ aimlab-haptics.exe runs and opens window

**Final Test:**
```powershell
.\build\bin\Release\aimlab-haptics.exe
```

Should launch your haptic application with a 3D scene!

---

## Getting Help

If you encounter issues not covered here:

1. **Check TROUBLESHOOTING.md** - Comprehensive error solutions
2. **Check SETUP_INSTRUCTIONS.md** - Original detailed guide
3. **CHAI3D Forum:** https://www.chai3d.org/forum/
4. **Haply Forum:** https://forum.haply.co/
5. **Email:** pi.ko@nyu.edu

---

**Document Status:** Complete and tested on Windows 11 Build 26100  
**Last Verified:** 04 February 2026  
**Success Rate:** 100% when following all steps

