# AIMLAB-Unity-Chai3D-VR

**Experimental Setup for Unity Chai3D VR non-Quest Link workflow**

---

## Project Overview

This project integrates CHAI3D haptic framework with Haply Inverse3 device and Unity VR for advanced haptic research and development. The repository provides a complete development environment for creating haptic-enabled VR applications.

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Date:** 04 February 2026  
**Version:** v1.1

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

3. **Build this project:**
   ```powershell
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

4. **Run the application:**
   ```powershell
   .\bin\Release\aimlab-haptics.exe
   ```

---

## Documentation

Complete setup instructions, troubleshooting guide, and API references are available in:

**📖 [docs/SETUP_INSTRUCTIONS.md](docs/SETUP_INSTRUCTIONS.md)**

This comprehensive guide covers:
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
