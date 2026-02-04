# AIMLAB-Unity-Chai3D-VR

**Experimental Setup for Unity Chai3D VR non-Quest Link workflow**

---

## Project Overview

This project integrates CHAI3D haptic framework with Haply Inverse3 device and Unity VR for advanced haptic research and development. The repository provides a complete development environment for creating haptic-enabled VR applications.

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 February 2026  
**Version:** v1.6

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
2. **CMake** >= 3.10 (bundled with VS2022 or standalone)
   ```powershell
   winget install Kitware.CMake
   ```
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
   ```powershell
   .\bin\Release\aimlab-haptics.exe
   ```

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
- ✅ CMake build system for cross-platform development
- ✅ Comprehensive documentation

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
