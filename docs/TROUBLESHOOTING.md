/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Troubleshooting Guide
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Comprehensive troubleshooting guide for common issues with CHAI3D,
 *   Haply Inverse3 device, and the AIMLAB haptic application.
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial troubleshooting guide
 * 
 ****************************************************************************/

# AIMLAB Haptics - Troubleshooting Guide

---

## Table of Contents

1. [Device Connection Issues](#device-connection-issues)
2. [Build and Compilation Errors](#build-and-compilation-errors)
3. [Runtime Errors](#runtime-errors)
4. [Performance Issues](#performance-issues)
5. [Graphics and Rendering](#graphics-and-rendering)
6. [Haptic Quality Issues](#haptic-quality-issues)

---

## Device Connection Issues

### Problem: Device Not Detected

**Symptoms:**
```
ERROR: No haptic device detected!
```

**Solutions:**

1. **Check Physical Connection**
   ```powershell
   # Verify device shows up in Windows Device Manager
   devmgmt.msc
   # Look for: Ports (COM & LPT) → USB Serial Device
   ```

2. **Check Haply Hub**
   - Open Haply Hub application
   - Device should be listed with device ID (e.g., INV3-XXXX)
   - Check Inverse Service status (should be running)
   - Service version must be >= 3.1.0

3. **Verify USB Connection**
   - Status LED sequence: Red (no USB) → Purple (USB connected)
   - Try different USB port (use USB 3.0+ directly on motherboard)
   - Avoid USB hubs and dongles
   - Use the cable provided by Haply

4. **Power Supply**
   - Ensure 24V power supply is connected
   - Check barrel jack is firmly inserted
   - LED should be steady (not blinking)

5. **Calibration**
   - Touch magnet to calibration point on device body
   - LED should turn white when calibrated
   - Re-calibrate if device was moved or unplugged

6. **Restart Services**
   ```powershell
   # Restart Haply Inverse Service
   Get-Service -Name "*haply*" | Restart-Service
   
   # Or restart Haply Hub application
   taskkill /IM "Haply Hub.exe" /F
   # Then relaunch Haply Hub from Start Menu
   ```

### Problem: Device Detected but No Haptic Feedback

**Possible Causes:**

1. **Device Not Calibrated**
   - Calibrate by touching magnet to calibration point
   - Wait for white LED

2. **Workspace Out of Range**
   - Move cursor within workspace bounds:
     - X: ±80mm, Y: ±50mm, Z: 0-130mm
   - Device has software limits to prevent damage

3. **Force Magnitude Too Small**
   ```cpp
   // Increase stiffness
   object->m_material->setStiffness(2000.0);  // Try higher value
   ```

4. **Haptic Thread Not Running**
   - Check console output for "Starting haptic rendering thread..."
   - Verify no crashes in haptic thread

---

## Build and Compilation Errors

### Problem: CMake Can't Find CHAI3D

**Error:**
```
CMake Error: Could not find a package configuration file provided by "CHAI3D"
```

**Solution:**

1. **Build CHAI3D First**
   ```powershell
   cd external/chai3d
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON
   cmake --build . --config Release
   ```

2. **Verify CHAI3D Build**
   ```powershell
   # Check if library exists
   Test-Path external\chai3d\build\Release\chai3d.lib
   ```

3. **Clear CMake Cache**
   ```powershell
   Remove-Item build\CMakeCache.txt
   Remove-Item -Recurse build\CMakeFiles
   ```

### Problem: Linker Errors (Unresolved Externals)

**Error:**
```
unresolved external symbol "class chai3d::cVector3d"
```

**Solutions:**

1. **Architecture Mismatch**
   - Ensure both CHAI3D and your app are built for x64
   - Always use: `-A x64` in CMake commands

2. **Configuration Mismatch**
   - If CHAI3D built in Release, build your app in Release
   - Mixing Debug/Release can cause linker errors

3. **Missing ENABLE_HAPLY_DEVICES Flag**
   ```powershell
   # Rebuild CHAI3D with correct flag
   cd external\chai3d\build
   cmake .. -DENABLE_HAPLY_DEVICES=ON
   cmake --build . --config Release
   ```

### Problem: FreeGLUT Not Found

**Error:**
```
fatal error: GL/freeglut.h: No such file or directory
```

**Solution:**

FreeGLUT is bundled with CHAI3D. Check:

```powershell
# Verify FreeGLUT exists
Test-Path external\chai3d\external\freeglut\include\GL\freeglut.h
```

If missing, the CHAI3D submodule may not be fully initialized:

```powershell
cd external\chai3d
git submodule update --init --recursive
```

### Problem: Visual Studio Not Found

**Error:**
```
CMake Error: CMAKE_C_COMPILER not set
```

**Solutions:**

1. **Use Developer PowerShell for VS 2022**
   - Start Menu → "Developer PowerShell for VS 2022"
   - This sets up MSVC compiler paths

2. **Specify Generator Explicitly**
   ```powershell
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

3. **Verify VS 2022 Installation**
   - Open Visual Studio Installer
   - Ensure "Desktop development with C++" workload is installed

---

## Runtime Errors

### Problem: Application Crashes on Startup

**Diagnostic Steps:**

1. **Run in Debug Mode**
   ```powershell
   .\build-project.ps1 -Config Debug
   cd build\bin\Debug
   .\aimlab-haptics.exe
   ```

2. **Check Console Output**
   - Look for error messages before crash
   - Note which initialization step fails

3. **Common Crash Causes**

   **Missing DLLs:**
   ```
   Error: The code execution cannot proceed because freeglut.dll was not found
   ```
   Solution: Copy DLL to executable directory
   ```powershell
   Copy-Item external\chai3d\external\freeglut\bin\x64\freeglut.dll build\bin\Release\
   ```

   **OpenGL Errors:**
   - Update GPU drivers
   - Check OpenGL version: `glxinfo` (Linux) or GPU-Z (Windows)

   **Device Access Error:**
   - Another application may have exclusive device access
   - Close all running CHAI3D demos
   - Restart Haply Inverse Service

### Problem: Haptic Thread Crashes

**Error:**
```
Exception in haptic thread: Access violation
```

**Solutions:**

1. **Null Pointer Check**
   ```cpp
   // Always check device pointer before use
   if (device != nullptr && device->isDeviceReady()) {
       device->getPosition(position);
   }
   ```

2. **Race Condition**
   - Ensure proper initialization order
   - Device must be opened before starting haptic thread

3. **Force Magnitude Check**
   ```cpp
   // Clamp forces to device maximum
   if (force.length() > info.m_maxLinearForce) {
       force.normalize();
       force *= info.m_maxLinearForce;
   }
   ```

---

## Performance Issues

### Problem: Low Haptic Update Rate

**Symptoms:**
- Jittery or unstable haptic feedback
- Frequency counter shows < 1000 Hz

**Solutions:**

1. **Use Release Build (Most Important!)**
   ```powershell
   cmake --build . --config Release  # NOT Debug!
   ```
   Debug builds are 10x slower and cannot maintain 1 kHz.

2. **Minimize Haptic Loop Computation**
   ```cpp
   // ✅ Good - minimal computation
   void updateHaptics() {
       while (running) {
           tool->updateFromDevice();
           tool->computeInteractionForces();
           tool->applyToDevice();
       }
   }
   
   // ❌ Bad - too much computation
   void updateHaptics() {
       while (running) {
           // Console I/O - very slow!
           cout << "Position: " << position << endl;
           
           // File I/O - extremely slow!
           logFile << position << endl;
           
           // Memory allocation - unpredictable timing
           std::vector<double> data;
           data.push_back(force.length());
       }
   }
   ```

3. **Reduce Scene Complexity**
   - Use simpler collision meshes
   - Reduce polygon count: `mesh->reduceNumberOfTriangles(1000);`
   - Disable unnecessary objects: `object->setHapticEnabled(false);`

4. **USB Connection**
   - Use USB 3.0+ port directly on motherboard
   - Avoid USB hubs, dongles, and extension cables
   - Check CPU usage - other processes may be interfering

### Problem: Graphics Lag / Low FPS

**Solutions:**

1. **Separate Threads**
   - Haptic thread: High priority, minimal computation
   - Graphics thread: Normal priority, can be slower (30-60 FPS is fine)

2. **Use Display Lists**
   ```cpp
   object->setUseDisplayList(true);
   ```

3. **Reduce Rendering Quality**
   ```cpp
   // Lower mesh detail
   mesh->setDisplayLevel(1);
   
   // Disable shadows (if enabled)
   light->setShadowMapEnabled(false);
   ```

---

## Graphics and Rendering

### Problem: Black Screen / Nothing Renders

**Solutions:**

1. **Check OpenGL Drivers**
   ```powershell
   # Update Intel graphics
   winget upgrade Intel.GraphicsDriver
   ```

2. **Verify Camera Setup**
   ```cpp
   // Camera must be positioned away from origin
   camera->set(
       cVector3d(0.5, 0.0, 0.3),    // Not (0,0,0)!
       cVector3d(0.0, 0.0, 0.0),    // Looking at origin
       cVector3d(0.0, 0.0, 1.0)     // Up is +Z
   );
   
   // Check clipping planes
   camera->setClippingPlanes(0.01, 10.0);  // Near, far
   ```

3. **Verify Lighting**
   ```cpp
   // Light must be enabled and added to world
   light->setEnabled(true);
   world->addChild(light);
   ```

4. **Check Object Visibility**
   ```cpp
   object->setShowEnabled(true);
   world->addChild(object);  // Must be in scene graph!
   ```

### Problem: Objects Render but No Colors/Materials

**Solutions:**

```cpp
// Enable material rendering
object->setUseMaterial(true);

// Set material color
object->m_material->setRed();

// Check lighting is enabled
light->setEnabled(true);

// Verify normals exist
mesh->computeAllNormals();
```

---

## Haptic Quality Issues

### Problem: Vibration / Oscillation

**Causes:**
- Stiffness too high for update rate
- Collision detection instability
- Friction/damping too low

**Solutions:**

1. **Reduce Stiffness**
   ```cpp
   // Lower stiffness for stability
   material->setStiffness(500.0);  // Instead of 3000.0
   ```

2. **Add Damping**
   ```cpp
   material->setViscosity(5.0);  // Add viscous damping
   ```

3. **Increase Update Rate**
   - Use Release build
   - Simplify haptic loop
   - Check frequency counter: should be >= 1000 Hz

### Problem: Force Feels Wrong / Weak

**Solutions:**

1. **Check Force Direction**
   ```cpp
   // Forces should oppose motion into object
   // Use device->setForce() carefully
   ```

2. **Verify Stiffness Range**
   ```cpp
   // Inverse3 recommended range: 100 - 3000 N/m
   material->setStiffness(1500.0);  // Mid-range
   ```

3. **Workspace Scaling**
   ```cpp
   // Adjust tool workspace to match device
   tool->setWorkspaceRadius(0.15);  // Meters
   ```

### Problem: "Pop Through" Objects

**Symptoms:**
- Cursor passes through solid objects
- No resistance at surface

**Solutions:**

1. **Collision Detection**
   ```cpp
   // For meshes: create AABB tree
   mesh->createAABBCollisionDetector(toolRadius);
   
   // For primitives: should work automatically
   sphere->setHapticEnabled(true);
   ```

2. **Tool Radius**
   ```cpp
   // Smaller radius = better precision
   tool->setRadius(0.003);  // 3mm
   ```

3. **Stiffness Too Low**
   ```cpp
   material->setStiffness(2000.0);  // Increase
   ```

---

## Common Error Messages

### "Failed to open device connection"

**Checklist:**
- [ ] Device powered on (24V connected)
- [ ] USB cable connected (purple LED)
- [ ] Device calibrated (white LED)
- [ ] Haply Hub running
- [ ] No other application using device
- [ ] COM port accessible (check Device Manager)

### "Haptic thread frequency too low"

**Fix:**
```powershell
# Always use Release build for haptics
cmake --build . --config Release
```

### "OpenGL version not supported"

**Fix:**
```powershell
# Update GPU drivers
# Intel:
winget upgrade Intel.GraphicsDriver

# NVIDIA:
# Download from https://www.nvidia.com/Download/index.aspx
```

### "freeglut.dll not found"

**Fix:**
```powershell
# Copy FreeGLUT DLL to executable directory
Copy-Item external\chai3d\external\freeglut\bin\x64\freeglut.dll build\bin\Release\
```

---

## Advanced Diagnostics

### Enable Verbose Logging

```cpp
// In main.cpp, before device operations
#define CHAI3D_VERBOSE_MODE
#include "chai3d.h"

// This enables detailed console output for debugging
```

### Monitor Haptic Performance

```cpp
// Add to haptic thread
cFrequencyCounter freqCounter;

void updateHaptics() {
    while (running) {
        freqCounter.signal(1);
        
        // ... normal haptic loop ...
        
        // Print every 1000 iterations
        static int counter = 0;
        if (++counter % 1000 == 0) {
            double freq = freqCounter.getFrequency();
            if (freq < 900.0) {
                cerr << "WARNING: Haptic rate low: " << freq << " Hz" << endl;
            }
        }
    }
}
```

### Test Device Directly

Create a minimal test program:

```cpp
#include "chai3d.h"
#include <iostream>

int main() {
    cHapticDeviceHandler handler;
    cGenericHapticDevicePtr device;
    
    handler.getDevice(device, 0);
    
    if (device == nullptr) {
        std::cerr << "No device!" << std::endl;
        return 1;
    }
    
    device->open();
    std::cout << "Device opened: " << device->getSpecifications().m_modelName << std::endl;
    
    cVector3d pos;
    device->getPosition(pos);
    std::cout << "Position: " << pos.str(3) << std::endl;
    
    device->close();
    return 0;
}
```

Build and run this to isolate device vs. scene issues.

---

## Windows-Specific Issues

### Developer PowerShell Not Found

**Solution:**

Install Visual Studio 2022 with C++ workload, or use regular PowerShell with:

```powershell
# Manually set up MSVC environment
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
```

### Permission Errors

**Error:**
```
Access denied when accessing COM port
```

**Solution:**

Run PowerShell as Administrator:
```powershell
Start-Process powershell -Verb RunAs
```

### Antivirus Blocking

Some antivirus software blocks USB serial communication:

1. Add exception for Haply Hub
2. Add exception for your project executables
3. Temporarily disable antivirus for testing

---

## Still Having Issues?

### Collect Diagnostic Information

```powershell
# System info
systeminfo | Select-String "OS Name", "OS Version", "System Type", "Processor"

# CMake version
cmake --version

# Visual Studio version
Get-ItemProperty "HKLM:\SOFTWARE\Microsoft\VisualStudio\*" | Select-Object DisplayName, DisplayVersion

# Git version
git --version

# Haply Hub version
# (Check in Haply Hub application → Settings → About)

# Device Manager - COM ports
Get-PnpDevice -Class Ports | Where-Object {$_.Status -eq "OK"}
```

### Check Log Files

```powershell
# CHAI3D logs (if verbose mode enabled)
# Check console output from your application

# Haply Inverse Service logs
# Location: C:\ProgramData\Haply\logs\
Get-ChildItem C:\ProgramData\Haply\logs\ -Recurse
```

### Get Help

1. **Check Documentation**
   - `docs/SETUP_INSTRUCTIONS.md` - Complete setup
   - `docs/BUILD_GUIDE.md` - Build process
   - `docs/DEVELOPMENT.md` - API and examples

2. **Search Known Issues**
   - Haply Forum: https://forum.haply.co/
   - CHAI3D Forum: https://www.chai3d.org/forum/

3. **Report Bug**
   - GitHub Issues (this repository)
   - Include:
     - Error messages (full text)
     - System information
     - Steps to reproduce
     - CMake output
     - Console logs

4. **Contact**
   - Email: pi.ko@nyu.edu
   - Haply Support: support@haply.co

---

## Prevention Tips

### Best Practices to Avoid Issues

✅ **Always use Release builds for haptics**  
✅ **Keep haptic loop minimal and fast**  
✅ **Test with simple scenes first**  
✅ **Calibrate device before each session**  
✅ **Keep USB connection direct (no hubs)**  
✅ **Update drivers and software regularly**  
✅ **Use version control (git) for your code**  
✅ **Read error messages carefully**  

---

## Emergency Recovery

### Nuclear Option - Complete Reset

If all else fails, start fresh:

```powershell
# 1. Remove all build artifacts
Remove-Item -Recurse -Force build
Remove-Item -Recurse -Force external\chai3d\build

# 2. Reinstall Haply Hub
# Download fresh installer from:
# https://develop.haply.co/releases/manager/latest

# 3. Restart computer

# 4. Re-run setup
.\setup-chai3d.ps1
.\build-project.ps1
```

---

**Remember:** Haptic development requires patience. Most issues have simple solutions. Take it step by step!

---

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Last Updated:** 04 February 2026
