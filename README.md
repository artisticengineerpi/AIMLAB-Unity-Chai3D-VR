# AIMLAB-Unity-Chai3D-VR

**Experimental Setup for Unity Chai3D VR non-Quest Link workflow**

---

## Project Overview

This project integrates CHAI3D haptic framework with Haply Inverse3 device and Unity VR for advanced haptic research and development. The repository provides a complete development environment for creating haptic-enabled VR applications.

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 February 2026  
**Version:** v2.4

---

## Project Structure

```
AIMLAB-Unity-Chai3D-VR/
├── README.md              # This file
├── CMakeLists.txt         # Top-level CMake configuration
├── .gitignore             # Git ignore rules
├── .gitattributes         # Git attributes
├── docs/                  # Documentation
│   └── SETUP_INSTRUCTIONS.md  # Complete setup guide
├── external/              # Third-party dependencies
│   ├── chai3d/            # CHAI3D library (Haply fork, git submodule)
│   └── HardwareAPI/       # Haply HardwareAPI C++ (optional)
├── src/                   # Application source code
│   └── main.cpp           # Starter haptic application
├── resources/             # 3D models, textures, sounds
├── bin/                   # Built executables (gitignored)
│   └── resources/         # Runtime resources
└── build/                 # CMake build directory (gitignored)
```

---

## Quick Start

### Prerequisites

1. **Visual Studio 2022** with "Desktop development with C++" workload
2. **CMake** >= 3.15 (required for runtime library control)
   ```powershell
   winget install Kitware.CMake
   ```
   **Note:** CMake 3.15+ is required for `CMAKE_MSVC_RUNTIME_LIBRARY` to properly set `/MT` flags.
3. **Git** for Windows
4. **Haply Hub** and Inverse Service >= 3.1.0
5. **Haply Inverse3** device (connected, powered, calibrated)

### Setup Steps

1. **Clone CHAI3D as submodule:**
   ```powershell
   git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d
   cd external/chai3d
   git checkout haply-api-cpp
   git submodule update --init --recursive
   cd ../..
   ```

   **⚠️ Important: SSH to HTTPS Conversion**
   
   If the submodule initialization fails with an SSH error (common if you don't have SSH keys set up), CHAI3D's nested submodules use SSH URLs. Configure Git to automatically convert SSH to HTTPS:
   
   ```powershell
   # Run this once (global setting)
   git config --global url."https://github.com/".insteadOf "git@github.com:"
   ```
   
   Then retry the submodule initialization:
   
   ```powershell
   cd external/chai3d
   git submodule update --init --recursive
   cd ../..
   ```
   
   This makes Git automatically translate any `git@github.com:` SSH URL into `https://github.com/` before connecting, which works without SSH keys.
   
   > **Note:** To undo this later (e.g., after setting up SSH keys), run:
   > ```powershell
   > git config --global --unset url."https://github.com/".insteadOf
   > ```

2. **Build CHAI3D library:**
   ```powershell
   cd external/chai3d
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON
   cmake --build . --config Release
   cd ../../..
   ```

   **⚠️ Important: CMake 4.x Compatibility Fix**
   
   If you're using CMake 4.x and encounter this error:
   ```
   CMake Error at CMakeLists.txt:44 (cmake_minimum_required):
     Compatibility with CMake < 3.5 has been removed from CMake.
   ```
   
   CMake 4.x dropped support for the old minimum version that CHAI3D's CMakeLists.txt declares. Add the policy override flag that CMake suggests:
   
   ```powershell
   cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
   ```
   
   **Note:** The quotes around `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` are important! PowerShell splits arguments at the dot (`.`), treating `.5` as a separate path argument. The quotes tell PowerShell to pass it as one complete argument to CMake.
   
   This tells CMake "yes, this project was written for an older CMake, configure it anyway." The CHAI3D code itself compiles fine — it's just the version declaration in their CMakeLists.txt that's outdated.

3. **Build this project:**
   ```powershell
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

   **📝 Note: C++14 Standard**
   
   This project uses C++14 (not C++17) to maintain compatibility with CHAI3D's bundled Eigen library. CHAI3D's Eigen uses `std::binder1st` and `std::binder2nd`, which were removed in C++17. The CMakeLists.txt is already configured for C++14, so no additional flags are needed.

4. **Run the application:**
   
   **Option A: Using the automated run script (Recommended)**
   ```powershell
   .\run.ps1
   ```
   
   This script automatically:
   - Kills `haply-inverse-service` process
   - Stops Haply Hub and related processes
   - Stops Haply Windows services
   - Launches your application
   
   **Why use this?** The Haply Inverse Service holds exclusive COM port access, causing "error 5 (ACCESS_DENIED)" when CHAI3D tries to connect via HardwareAPI. This script cleans up before launching.
   
   **Option B: Manual launch**
   ```powershell
   # Kill Haply processes first
   Stop-Process -Name "haply-inverse-service" -Force -ErrorAction SilentlyContinue
   
   # Then run
   .\build\Release\aimlab-haptics.exe
   ```

---

## Automation Scripts

This project includes three PowerShell automation scripts for convenience:

### 🔧 **setup-chai3d.ps1** - Initial CHAI3D Setup
```powershell
.\setup-chai3d.ps1
```
- Clones CHAI3D as git submodule
- Checks out haply-api-cpp branch
- Initializes nested submodules
- Builds CHAI3D library with Haply support
- **Run once during initial setup** (~15 minutes)

### 🔨 **rebuild.ps1** - Clean Rebuild
```powershell
.\rebuild.ps1
```
- Deletes `build/` directory
- Creates fresh build directory
- Runs CMake configuration
- Builds project in Release mode
- Optionally launches app after building
- **Use when making major changes** (~30-60 seconds)

### ▶️ **run.ps1** - Launch with Process Cleanup
```powershell
.\run.ps1
```
- Kills Haply processes holding COM port
- Stops Haply services
- Launches application
- **Use every time you run the app** (instant)

### 🔄 **build-project.ps1** - Incremental Build
```powershell
.\build-project.ps1
```
- Builds without cleaning
- Faster for small code changes
- Keeps existing build artifacts
- **Use for quick iterations** (~10-30 seconds)

---

## Documentation

### Quick Start Guides

**🚀 [docs/REAL_WORLD_SETUP_GUIDE.md](docs/REAL_WORLD_SETUP_GUIDE.md)** ⭐ **START HERE!**
- **Practical guide based on actual implementation experience**
- Real issues encountered and their solutions
- Step-by-step with all the gotchas documented
- Tested on Windows 11 Build 26100
- Perfect for LLM agents and developers

**📖 [docs/SETUP_INSTRUCTIONS.md](docs/SETUP_INSTRUCTIONS.md)**
- Complete theoretical setup guide
- Architecture overview and design decisions
- All 31 demos explained
- Extension modules (GEL, BULLET, ODE)
- Reference documentation

### Additional Resources

This comprehensive documentation suite covers:
- Complete installation and configuration
- CHAI3D library building and modules
- Haply device setup and calibration
- Creating custom haptic applications
- Troubleshooting common issues
- Reference links and resources

---

## Features

- ✅ CHAI3D haptic framework integration
- ✅ Haply Inverse3 device support (via Haply fork)
- ✅ Real-time haptic rendering (1 kHz+)
- ✅ GLUT-based 3D graphics
- ✅ Starter application with interactive sphere
- ✅ **Graceful device fallback** - runs in visual-only mode without device
- ✅ CMake build system for cross-platform development
- ✅ Comprehensive documentation with 10+ common issues solved

---

## Hardware Requirements

- **Haply Inverse3** haptic device
- **Windows 11** x64 (tested on i9-14900HX / 32 GB RAM)
- **OpenGL-capable GPU** (Intel UHD Graphics or better)
- **USB-C port** (direct connection, no hubs)
- **24V power supply** (included with Inverse3)

---

## Software Stack

- **CHAI3D** v3.3.2 (Haply fork with Inverse3 support)
- **Haply Inverse SDK** >= 3.1.0 (service-based architecture)
- **FreeGLUT** (bundled with CHAI3D)
- **OpenGL** (system-provided)
- **MSVC v143** toolset (Visual Studio 2022)

---

## Common Issues & Solutions

### Issue 1: CMake Not Found After Installation

**Problem:**
```powershell
cmake : The term 'cmake' is not recognized...
```

**Solution:** Refresh PATH in current PowerShell session:
```powershell
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
cmake --version  # Verify it works
```

Or close and reopen PowerShell.

---

### Issue 2: Submodule SSH Authentication Failure

**Problem:**
```
Host key verification failed.
fatal: Could not read from remote repository.
```

**Solution:** Configure Git to convert SSH URLs to HTTPS (already documented in Setup Steps above):
```powershell
git config --global url."https://github.com/".insteadOf "git@github.com:"
git submodule update --init --recursive
```

---

### Issue 3: CMake 4.x Compatibility Error

**Problem:**
```
CMake Error: Compatibility with CMake < 3.5 has been removed from CMake.
```

**Solution:** Add policy override flag with proper quoting (already documented in Setup Steps above):
```powershell
cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
```

**Important:** Quotes around the entire flag prevent PowerShell from splitting at the dot!

---

### Issue 4: Eigen Binder Errors (C++17)

**Problem:**
```
error C2039: 'binder1st': is not a member of 'std'
error C2039: 'binder2nd': is not a member of 'std'
```

**Solution:** This project uses C++14 (already configured in CMakeLists.txt). CHAI3D's Eigen uses `std::binder1st/binder2nd` which were removed in C++17.

---

### Issue 5: FreeGLUT Header Not Found

**Problem:**
```
error C1083: Cannot open include file: 'GL/freeglut.h'
```

**Solution:** FreeGLUT is automatically built from source by CMakeLists.txt. Make sure you're using the latest version from this repository.

---

### Issue 6: FreeGLUT Library Linker Error

**Problem:**
```
LINK : fatal error LNK1181: cannot open input file 'freeglut.lib'
```

**Solution:** FreeGLUT is built from source using `add_subdirectory()` in CMakeLists.txt (already configured). Clean rebuild:
```powershell
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build . --config Release
```

---

### Issue 7: Wrong CHAI3D Class Names

**Problem:**
```
error C2061: syntax error: identifier 'cSphere'
```

**Solution:** Use `cShapeSphere` instead of `cSphere` (already fixed in src/main.cpp).

---

### Issue 8: Specular Material Property Error

**Problem:**
```
error: 'class chai3d::cMaterial' has no member named 'setSpecularLevel'
```

**Solution:** Use direct property access (already fixed in src/main.cpp):
```cpp
sphere->m_material->m_specular.set(0.8f, 0.8f, 0.8f);
```

---

### Issue 9: Device Not Detected

**Problem:**
```
Could not open serial port: IO Exception: error code 5
```

**Cause:** Error code 5 = ACCESS_DENIED

**Solutions:**
1. **Close Haply Hub** - It locks the COM port (conflicts with CHAI3D's HardwareAPI)
2. **Check connections:**
   - USB-C plugged in (LED: red → purple)
   - 24V power connected
   - Device calibrated (touch magnet → white LED)
3. **Run as Administrator** if needed

---

### Issue 10: Runtime Library Mismatch (LNK2038)

**Problem:**
```
error LNK2038: mismatch detected for 'RuntimeLibrary': 
  value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'
```

**Cause:** CHAI3D was built with static runtime (`/MT`), but your project is using dynamic runtime (`/MD`). They must match!

**Solution:** Already fixed in CMakeLists.txt. The project uses:
- **CMake 3.15 minimum** (required for CMAKE_MSVC_RUNTIME_LIBRARY support)
- **Static runtime setting** to match CHAI3D:
```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```

This sets `/MT` for Release and `/MTd` for Debug.

**Critical: You MUST do a clean rebuild after pulling this fix:**
```powershell
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
cmake --build . --config Release
```

---

### Quick Troubleshooting Reference

| Error | Quick Fix |
|-------|-----------|
| `cmake` not recognized | Refresh PATH: `$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")` |
| SSH submodule failure | `git config --global url."https://github.com/".insteadOf "git@github.com:"` |
| CMake < 3.5 error | Add flag: `"-DCMAKE_POLICY_VERSION_MINIMUM=3.5"` (with quotes!) |
| Eigen binder errors | Use C++14 (already set in CMakeLists.txt) |
| GL/freeglut.h not found | Clean rebuild - FreeGLUT builds automatically |
| freeglut.lib not found | Clean rebuild - FreeGLUT builds from source |
| Runtime library mismatch (LNK2038) | Requires CMake 3.15+ and clean rebuild - uses /MT to match CHAI3D |
| Serial port error 5 | Close Haply Hub before running |

**For detailed troubleshooting:** See [docs/REAL_WORLD_SETUP_GUIDE.md](docs/REAL_WORLD_SETUP_GUIDE.md)

---

## License

- **CHAI3D core:** BSD 3-Clause
- **Haply device support:** GPLv3 (when ENABLE_HAPLY_DEVICES=ON)
- **Haply HardwareAPI:** GPLv3
- **This project:** See LICENSE file

---

## Support & Resources

- **Haply Documentation:** https://docs.haply.co/
- **CHAI3D Homepage:** https://www.chai3d.org/
- **Haply CHAI3D Fork:** https://github.com/HaplyHaptics/chai3d
- **Haply Forum:** https://forum.haply.co/

---

## Changelog

### v2.4 - 04 February 2026
- Added run.ps1 script with automatic Haply process cleanup
- Updated rebuild.ps1 to optionally launch app after building
- Documented all four automation scripts (setup, rebuild, run, build)
- Added Automation Scripts section to README with usage instructions
- Resolves COM port ACCESS_DENIED by killing haply-inverse-service automatically

### v2.3 - 04 February 2026
- Added graceful device error handling - runs in visual-only mode without device
- Application provides helpful diagnostics for device connection failures
- Updated features list with graceful fallback capability
- Main.cpp v1.4 with three-tier device detection

### v2.2 - 04 February 2026
- Updated CMake minimum version requirement to 3.15 (for CMAKE_MSVC_RUNTIME_LIBRARY)
- Clarified runtime library fix requires CMake 3.15+
- Application now builds successfully with all fixes applied
- Project is fully tested and functional

### v2.1 - 04 February 2026
- Added Issue 10: Runtime library mismatch (LNK2038) with solution
- CMakeLists.txt now uses static /MT runtime to match CHAI3D
- Updated quick reference table with all 10 common issues

### v2.0 - 04 February 2026
- Added comprehensive "Common Issues & Solutions" section to README
- Complete troubleshooting reference table with 9 common issues
- All real-world problems documented with quick fixes
- FreeGLUT now builds from source via add_subdirectory()
- Project is fully functional and tested end-to-end

### v1.6 - 04 February 2026
- Fixed FreeGLUT path in CMakeLists.txt (extras/freeglut not external/freeglut)
- Added REAL_WORLD_SETUP_GUIDE.md with actual implementation experience
- Documented all real-world issues and solutions encountered during setup

### v1.5 - 04 February 2026
- Changed C++ standard from C++17 to C++14 for CHAI3D Eigen compatibility
- Added note about C++14 requirement (std::binder1st/binder2nd removed in C++17)
- Updated CMakeLists.txt with compatibility comments

### v1.4 - 04 February 2026
- Fixed PowerShell quoting for CMake flag to prevent argument splitting at dot
- Added note explaining why quotes are needed around -DCMAKE_POLICY_VERSION_MINIMUM=3.5

### v1.3 - 04 February 2026
- Added CMake 4.x compatibility fix with -DCMAKE_POLICY_VERSION_MINIMUM=3.5 flag
- Troubleshooting note for CHAI3D build with newer CMake versions

### v1.2 - 04 February 2026
- Added CMake installation command using winget

### v1.1 - 04 February 2026
- Added SSH to HTTPS conversion instructions for submodule initialization
- Troubleshooting note for users without SSH keys configured

### v1.0 - 04 February 2026
- Initial project structure
- CMake build system configuration
- Starter haptic application (main.cpp)
- Complete setup documentation
- Directory structure and .gitignore

---

## Contact

**Pi Ko**  
Email: pi.ko@nyu.edu  
Project: AIMLAB VR Haptics Research
