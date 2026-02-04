/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Using Official CHAI3D v3.3.5
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Guide for using the official Haply CHAI3D release (v3.3.5) with
 *   proper Inverse3 support. Explains differences from GitHub fork
 *   and how to integrate with your project.
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial official CHAI3D guide
 * 
 ****************************************************************************/

# Using Official CHAI3D v3.3.5 with Inverse3

## Overview

You've downloaded the **official Haply CHAI3D v3.3.5** from [develop.haply.co](https://develop.haply.co/releases/chai3d). This version has **proper Inverse3 support** using the Inverse SDK.

**Location:** `Chai3D.3.3.5\`  
**Demos:** 34 pre-built executables in `win-x64\`

---

## Key Differences: GitHub Fork vs Official Release

| Feature | GitHub Fork | Official CHAI3D v3.3.5 |
|---------|-------------|------------------------|
| **Source** | github.com/HaplyHaptics/chai3d | develop.haply.co |
| **Inverse3 Support** | ❌ No (wrong protocol) | ✅ Yes (Inverse SDK) |
| **Pantograph Support** | ✅ Yes | ✅ Yes |
| **Haply Hub** | Must be CLOSED | Must be RUNNING |
| **Protocol** | Direct serial (HardwareAPI old) | Inverse SDK (WebSocket) |
| **Position Reading** | Always 0,0,0 with Inverse3 | ✅ Works correctly |
| **Format** | Source code (compile yourself) | Pre-built executables |
| **GEL Demos** | Need to build modules | ✅ Included (4 demos) |

---

## What You Have Now

### Pre-Built Demos (34 total)

**Standard CHAI3D Demos (31):**
- `01-mydevice.exe` - Basic device test
- `02-multi-devices.exe` - Multiple devices
- `03-analytics.exe` - Performance monitoring
- `04-shapes.exe` - 3D shapes
- ... (27 more) ...
- `31-pointcloud.exe` - Point cloud visualization

**GEL Module Demos (4):**
- `00-GEL-haply.exe` ⭐ - **Haply Inverse3 specific demo**
- `01-GEL-membrane.exe` - Deformable membrane puncture
- `02-GEL-cell.exe` - Cell deformation
- `03-GEL-duck.exe` - Deformable duck

**Utilities (3):**
- `cfont.exe` - Font converter
- `cimage.exe` - Image converter
- `cshader.exe` - Shader compiler

---

## Step 1: Test Official Demos with Inverse3

### Prepare Inverse3

1. ✅ Connect USB-C (LED: purple)
2. ✅ Connect 24V power
3. ✅ Calibrate (touch magnet → white LED)
4. ✅ **Open Haply Hub** (must be running!)
5. ✅ Verify device shows in Hub

### Run Official Demos

**Option A: Use the launcher script (Recommended)**
```powershell
.\run-official-demos.ps1
```

**Option B: Run demos directly**
```powershell
cd Chai3D.3.3.5\win-x64

# Test basic device connection
.\01-mydevice.exe

# Try Haply-specific demo
.\00-GEL-haply.exe

# Try popular demos
.\13-primitives.exe
.\23-tooth.exe
```

### What to Expect

✅ Device detected as "Haply Inverse3" (not "Pantograph")  
✅ Position changes when you move the device  
✅ Force feedback works  
✅ All haptic interactions functional  

---

## Step 2: Understand the Official Integration

### How Official Demos Work

1. **Uses Inverse SDK (not direct serial)**
   - Connects via WebSocket: `ws://localhost:10001`
   - Haply Inverse Service handles device communication
   - No COM port conflicts
   - Works with Haply Hub running

2. **Device Initialization:**
   ```cpp
   // Official CHAI3D uses Inverse SDK wrapper
   cHapticDeviceHandler* handler = new cHapticDeviceHandler();
   handler->getDevice(device, 0);  // Auto-detects Inverse3 via SDK
   device->open();  // Connects through Inverse Service
   ```

3. **Position/Force:**
   - SDK provides 3-DOF position (x, y, z)
   - SDK accepts 3-DOF force commands
   - Real-time streaming via WebSocket

---

## Step 3: Next Steps for Your Project

### Option A: Use Official Demos As-Is (Easiest)

**Just run the official demos:**
```powershell
.\run-official-demos.ps1
```

**Benefits:**
- ✅ Inverse3 works immediately
- ✅ No coding needed
- ✅ All 34 demos available
- ✅ GEL deformable simulations included

**Limitations:**
- ❌ Can't customize code (pre-built only)
- ❌ No source code provided in this package

---

### Option B: Get Official Source Code (For Development)

**Clone official demo source from GitLab:**
```powershell
cd C:\Users\other\Documents\GitHub
git clone https://gitlab.com/Haply/public/chai3d-demos.git
cd chai3d-demos
```

**Study the code:**
- See how Inverse3 is initialized
- Understand Inverse SDK integration
- Copy patterns to your project

**Build from source:**
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

### Option C: Integrate Inverse SDK into Your Project (Advanced)

**Update your `src/main.cpp` to use Inverse SDK:**

1. **Keep using CHAI3D's device abstraction** (easiest)
   - Official CHAI3D library already wraps Inverse SDK
   - Just link against official CHAI3D instead of GitHub fork

2. **Or use Inverse SDK directly:**
   ```cpp
   // WebSocket connection to Inverse Service
   // Connect to ws://localhost:10001
   // Send/receive JSON for position and forces
   ```

**Requirements:**
- Inverse SDK >= 3.1.0 (installed with Haply Hub)
- Inverse Service running
- WebSocket client library (or use CHAI3D wrapper)

---

## Recommended Path: Hybrid Approach

### For Learning and Testing

**Use official demos:**
```powershell
.\run-official-demos.ps1
```

Test all demos to understand CHAI3D capabilities with Inverse3.

---

### For Your Custom Development

**Option 1: Wait for Source Code** (Simplest)

The official release you downloaded is **pre-built only**. To get source code:

```powershell
# Clone official demos (Inverse3 compatible)
git clone https://gitlab.com/Haply/public/chai3d-demos.git

# Study the code
# Copy initialization patterns
# Build your own app using official CHAI3D library
```

**Option 2: Use Inverse SDK Directly** (Most Control)

Bypass CHAI3D device layer entirely:
- Direct WebSocket connection to Inverse Service
- JSON-based protocol
- Full control over device communication
- See: https://docs.haply.co/inverseSDK/

---

## Current Project Status Summary

### What Works ✅

**Your Current Project (GitHub Fork):**
- ✅ Builds successfully
- ✅ Runs in visual-only mode
- ✅ Graphics and UI working
- ✅ Good for: Learning CHAI3D, Pantograph device, graphics development

**Official CHAI3D Demos:**
- ✅ Full Inverse3 support
- ✅ Position tracking works
- ✅ Force feedback works
- ✅ Good for: Testing Inverse3, understanding API, seeing examples

### What Needs Integration ⚠️

To make YOUR custom app work with Inverse3:
- Need to switch from GitHub fork to official CHAI3D library
- OR implement Inverse SDK integration
- OR study GitLab source and adapt

---

## Quick Start: Test Official Demos Now

1. **Make sure Haply Hub is RUNNING**
   ```powershell
   # Check if running
   Get-Process | Where-Object { $_.ProcessName -like "*haply*" }
   
   # If not running, start Haply Hub from Start Menu
   ```

2. **Run a demo:**
   ```powershell
   .\run-official-demos.ps1
   ```

3. **Try Haply-specific demo first:**
   - Select `00-GEL-haply.exe` from menu
   - This demo was built specifically for Inverse3
   - Should show immediate position tracking and force feedback

4. **Move your Inverse3 handle:**
   - Watch cursor move in 3D scene
   - Touch objects and feel forces
   - Verify everything works!

---

## Troubleshooting Official Demos

### Demo Opens But No Device

**Check:**
```powershell
# Is Haply Hub running?
Get-Process | Where-Object { $_.ProcessName -like "*haply*" }

# Is Inverse Service running?
Get-Service | Where-Object { $_.DisplayName -like "*inverse*" }

# Is device visible in Haply Hub?
# Open Haply Hub and check device list
```

### Device Detected But No Force

- Calibrate device (touch magnet to calibration point)
- Check 24V power is connected
- Try different demo (some are visual-only)

---

## Files Overview

### Your Project Now Has

**Official CHAI3D (Downloaded):**
- `Chai3D.3.3.5\` - Extracted official release
- `Chai3D.3.3.5.zip` - Original download (gitignored)

**GitHub Fork (Compiled):**
- `external/chai3d/` - Old fork for Pantograph
- `build/` - Your compiled project

**Scripts:**
- `run-official-demos.ps1` - Launch official demos (Inverse3)
- `run.ps1` - Launch YOUR app (GitHub fork)
- `rebuild.ps1` - Rebuild YOUR app
- `setup-chai3d.ps1` - Setup GitHub fork

---

## Next Actions

### Immediate (Testing)

```powershell
# Test official demos with Inverse3
.\run-official-demos.ps1
```

### Short Term (Learning)

```powershell
# Clone official source code
cd C:\Users\other\Documents\GitHub
git clone https://gitlab.com/Haply/public/chai3d-demos.git

# Study how they integrate Inverse3
```

### Long Term (Your App)

**Decision needed:** Choose one path:

1. **Path A:** Link your project against official CHAI3D library
2. **Path B:** Integrate Inverse SDK directly into your app
3. **Path C:** Keep GitHub fork for Pantograph/learning only

I can help you with any of these paths once you decide!

---

## Summary

✅ **Downloaded:** Official CHAI3D v3.3.5 with Inverse3 support  
✅ **Excluded from git:** Added to .gitignore (too large)  
✅ **Script created:** `run-official-demos.ps1` for easy testing  
✅ **Documentation:** All differences explained in README  

**Test your Inverse3 now:**
```powershell
.\run-official-demos.ps1
```

---

**Last Updated:** 04 February 2026  
**Status:** Ready to test Inverse3 with official demos!
