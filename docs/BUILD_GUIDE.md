/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Build Guide
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Quick reference guide for building the AIMLAB haptic application.
 *   For complete setup instructions, see SETUP_INSTRUCTIONS.md
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial build guide
 * 
 ****************************************************************************/

# AIMLAB Haptics - Quick Build Guide

## Step 1: Add CHAI3D Submodule

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

# Add Haply's CHAI3D fork as submodule
git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d

# Checkout the correct branch with Haply support
cd external/chai3d
git checkout haply-api-cpp

# Initialize nested submodules (includes Haply-API-cpp)
git submodule update --init --recursive

# Return to project root
cd ../..
```

## Step 2: Build CHAI3D Library

```powershell
# Navigate to CHAI3D directory
cd external/chai3d

# Create build directory
mkdir build
cd build

# Configure with CMake (ENABLE_HAPLY_DEVICES=ON is critical!)
cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON

# Build the library in Release mode (takes 5-10 minutes)
cmake --build . --config Release

# Optional: Build all demos and examples
# cmake --build . --config Release --target ALL_BUILD

# Return to project root
cd ../../..
```

## Step 3: Build Your Application

```powershell
# Create build directory in project root
mkdir build
cd build

# Configure your application
cmake .. -G "Visual Studio 17 2022" -A x64

# Build in Release mode (always use Release for haptics!)
cmake --build . --config Release
```

## Step 4: Run the Application

```powershell
# From the build directory
.\bin\Release\aimlab-haptics.exe

# Or from project root
.\build\bin\Release\aimlab-haptics.exe
```

## Pre-flight Checklist Before Running

1. ✅ Inverse3 mounted to desk clamp, arms unfolded
2. ✅ USB-C connected directly to PC (status light: red → purple)
3. ✅ 24V power supply connected
4. ✅ Device calibrated (magnet touched to calibration point, white LED)
5. ✅ Haply Hub running and showing the device
6. ✅ Inverse Service >= 3.1.0 active

## Troubleshooting

### CMake can't find CHAI3D
**Error:** `Could not find a package configuration file provided by "CHAI3D"`

**Solution:** Build CHAI3D first (Step 2). CMake registers CHAI3D in its package registry during the build process.

### Linker Errors
**Error:** `unresolved external symbol` errors

**Solution:** 
- Ensure you used `-A x64` flag (not x86)
- Verify CHAI3D was built in Release mode
- Check that `ENABLE_HAPLY_DEVICES=ON` was set when building CHAI3D

### Device Not Found
**Error:** `No haptic device detected!`

**Solution:**
1. Open Haply Hub and verify device is listed
2. Check USB-C connection (try different port)
3. Verify 24V power is connected
4. Calibrate device (white LED)
5. Restart Inverse Service if needed

### Build Takes Too Long
**Tip:** To build only the core library (not all 31 demos):

```powershell
cmake --build . --config Release --target chai3d
```

## Development Workflow

### Rebuilding After Code Changes

```powershell
cd build
cmake --build . --config Release
.\bin\Release\aimlab-haptics.exe
```

### Clean Build (if needed)

```powershell
# Remove build directory
cd ..
Remove-Item -Recurse -Force build

# Rebuild from scratch
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Opening in Visual Studio GUI

```powershell
# Open the generated solution file
start .\build\AIMLAB-Haptics.sln
```

Or in Visual Studio:
1. **File → Open → CMake...**
2. Select `CMakeLists.txt` in project root
3. Configure: **x64-Release**
4. **Build → Build All**

## Performance Notes

- **Always use Release mode** for haptic applications
- Debug mode introduces frame drops that affect haptic stability
- Haptic thread should maintain >= 1000 Hz update rate
- Monitor performance with CHAI3D's built-in analytics (see demo `03-analytics`)

## Additional Resources

- **Complete Setup:** `docs/SETUP_INSTRUCTIONS.md`
- **CHAI3D Docs:** https://www.chai3d.org/download/doc/html/
- **Haply Docs:** https://docs.haply.co/
