/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Quick Start Guide
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Fast-track setup guide to get started with CHAI3D and Haply Inverse3
 *   in under 30 minutes. For detailed instructions, see docs/SETUP_INSTRUCTIONS.md
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial quick start guide
 * 
 ****************************************************************************/

# AIMLAB Haptics - Quick Start (30 Minutes)

This guide gets you from zero to running your first haptic application.

---

## Prerequisites (5 minutes)

### Required Software

1. **Visual Studio 2022** with C++ workload
   - Download: https://visualstudio.microsoft.com/downloads/
   - During install: Check "Desktop development with C++"

2. **Git for Windows**
   ```powershell
   winget install Git.Git
   ```

3. **CMake** (if not bundled with VS2022)
   ```powershell
   winget install Kitware.CMake
   ```

4. **Haply Hub** (includes Inverse Service)
   - Download: https://develop.haply.co/releases/manager/latest
   - Install and run the application

### Hardware Setup

1. Mount Inverse3 to desk clamp
2. Connect USB-C cable to PC (LED: red → purple)
3. Connect 24V power supply
4. **Calibrate:** Touch magnet to calibration point (LED: white)
5. Verify device appears in Haply Hub

---

## Automated Setup (15 minutes)

### Option A: Use PowerShell Scripts (Recommended)

Open **Developer PowerShell for VS 2022** and run:

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

# Step 1: Setup CHAI3D (takes 10-15 min)
.\setup-chai3d.ps1

# Step 2: Build your application (takes 30 sec)
.\build-project.ps1

# Step 3: Run the application
.\build\bin\Release\aimlab-haptics.exe
```

**Done!** Your haptic application should now be running.

---

## Manual Setup (20 minutes)

If you prefer step-by-step control:

### 1. Clone CHAI3D (2 minutes)

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d
cd external/chai3d
git checkout haply-api-cpp
git submodule update --init --recursive
cd ../..
```

### 2. Build CHAI3D (10-15 minutes)

```powershell
cd external/chai3d
mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON
cmake --build . --config Release

cd ../../..
```

**☕ Coffee break** - This takes ~10 minutes

### 3. Build Your Application (1 minute)

```powershell
mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### 4. Run! (30 seconds)

```powershell
.\bin\Release\aimlab-haptics.exe
```

---

## First Run Experience

When you run the application, you should see:

```
========================================
AIMLAB Haptics Starter Application
Author: Pi Ko (pi.ko@nyu.edu)
Date: 04 February 2026
========================================

Creating 3D world...
Setting up camera...
Configuring lighting...
Detecting haptic devices...
Device found: Haply Inverse3
Manufacturer: Haply
Device connection opened successfully
Device calibrated successfully
Creating haptic cursor...
Creating scene objects...
Starting haptic rendering thread...

Application ready!
Controls:
  ESC / 'q' - Quit
  'f' - Fullscreen
  Mouse drag - Rotate view
```

A window opens showing a red sphere. Move the Inverse3 cursor to touch it - you should feel resistance!

---

## Troubleshooting Quick Fixes

### "No haptic device detected"
✅ Check Haply Hub shows your device  
✅ Restart Haply Hub  
✅ Re-calibrate device (white LED)

### "CMake can't find CHAI3D"
✅ Build CHAI3D first: `.\setup-chai3d.ps1`  
✅ Use Developer PowerShell for VS 2022

### Build fails with linker errors
✅ Ensure using x64 (not x86): `-A x64`  
✅ Build CHAI3D in Release mode  
✅ Check ENABLE_HAPLY_DEVICES=ON was set

### Black screen / no graphics
✅ Update GPU drivers  
✅ Try windowed mode (don't press 'f')

---

## Try the Demos (5 minutes)

CHAI3D includes 31 demo applications:

```powershell
# Option A: Use the demo runner script
.\run-demos.ps1

# Option B: Run directly
cd external\chai3d\build\bin\Release

.\01-mydevice.exe      # Basic device test
.\13-primitives.exe    # Geometric shapes
.\23-tooth.exe         # Dental simulation
```

---

## Next Steps

### Learn More

- **Complete setup guide:** `docs/SETUP_INSTRUCTIONS.md`
- **Build guide:** `docs/BUILD_GUIDE.md`
- **Development guide:** `docs/DEVELOPMENT.md`
- **Contributing:** `CONTRIBUTING.md`

### Extend Your Application

Edit `src/main.cpp` to add:
- More 3D objects
- Custom force effects
- Texture mapping
- 3D model loading

Rebuild with:
```powershell
.\build-project.ps1
```

### Try Advanced Features

- **Load 3D models:** See `docs/DEVELOPMENT.md` → "Loading 3D Models"
- **Custom materials:** See "Haptic Material Properties" section
- **Multi-device:** Support multiple Inverse3 devices

### Build Extension Modules

```powershell
# GEL module (deformable bodies)
cd external\chai3d\modules\GEL
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Run membrane demo
.\bin\Release\50-GEL-membrane.exe
```

---

## VS Code Integration

This project includes VS Code configuration:

- **Build task:** Ctrl+Shift+B (or Terminal → Run Build Task)
- **Run task:** Terminal → Run Task → "Run Application"
- **CMake integration:** CMake Tools extension recommended

Install recommended extensions:
```powershell
code --install-extension ms-vscode.cpptools
code --install-extension ms-vscode.cmake-tools
code --install-extension twxs.cmake
```

---

## Quick Command Reference

```powershell
# Setup (first time only)
.\setup-chai3d.ps1

# Build your app
.\build-project.ps1

# Clean build
.\build-project.ps1 -Clean

# Run your app
.\build\bin\Release\aimlab-haptics.exe

# Run demos
.\run-demos.ps1

# Or manually
cd external\chai3d\build\bin\Release
.\13-primitives.exe
```

---

## Getting Help

- **Documentation:** `docs/` folder
- **CHAI3D docs:** https://www.chai3d.org/download/doc/html/
- **Haply docs:** https://docs.haply.co/
- **Issues:** GitHub issues page
- **Email:** pi.ko@nyu.edu

---

## Success Checklist

After following this guide, you should have:

- [x] Visual Studio 2022 installed
- [x] CHAI3D library built with Haply support
- [x] AIMLAB haptic application built and running
- [x] Inverse3 device connected and responding
- [x] Felt haptic feedback when touching the sphere
- [x] CHAI3D demos accessible and runnable

**Congratulations! You're ready to develop haptic applications!** 🎉

---

**Next:** Dive into `docs/DEVELOPMENT.md` to learn how to create custom haptic effects and build advanced applications.
