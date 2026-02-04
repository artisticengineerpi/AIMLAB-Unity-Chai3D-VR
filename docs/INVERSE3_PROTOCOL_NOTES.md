/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Inverse3 Protocol Notes
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Important information about Haply device compatibility and protocol
 *   differences between the old Pantograph and newer Inverse3.
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial protocol documentation
 * 
 ****************************************************************************/

# Inverse3 Protocol and Compatibility Notes

## ⚠️ Critical Discovery: Protocol Mismatch

### The Issue

The GitHub CHAI3D fork (`HaplyHaptics/chai3d`) that this project uses was designed for the **old Haply Pantograph (2-DOF)** device, NOT the **Inverse3**.

**Symptoms:**
```
Device found: Haply Pantograph
[debug] Tool pos: 0, 0, 0  (never changes even when moving device)
```

### Root Cause

1. **GitHub Fork:** Uses `Haply-API-cpp` serial protocol (old Pantograph)
2. **Inverse3:** Uses completely different protocol via HardwareAPI or Inverse SDK
3. **Result:** Driver opens serial port but speaks wrong protocol → position always reads zero

---

## Device Compatibility Matrix

| Device | GitHub Fork Support | Official Haply CHAI3D | Protocol |
|--------|---------------------|----------------------|----------|
| **Haply Pantograph (2-DOF)** | ✅ Supported | ✅ Supported | Haply-API-cpp (serial) |
| **Haply Inverse3** | ❌ NOT Supported | ✅ Supported | HardwareAPI / Inverse SDK |
| **Wireless Verse Grip** | ❌ NOT Supported | ✅ Supported | Inverse SDK (WebSocket) |

---

## Solutions for Inverse3

### Option 1: Use Official Haply CHAI3D Releases (Recommended)

**Download Pre-Built Demos:**
- URL: https://develop.haply.co/releases/chai3d
- Latest: **CHAI3D v3.3.5** (June 2025)
- Includes: Pre-built Windows demos with proper Inverse3 support
- File: `Chai3D.3.3.5.zip`

**Benefits:**
- ✅ Works with Inverse3 out of the box
- ✅ No compilation needed
- ✅ Verify device works before coding

**Download and Test:**
```powershell
# Download from https://develop.haply.co/releases/chai3d/3.3.5
# Extract Chai3D.3.3.5.zip
# Run any demo .exe to test Inverse3
```

---

### Option 2: Use GitLab Source Code

**Official Inverse3-Compatible Demos:**
- Repository: https://gitlab.com/Haply/public/chai3d-demos
- Branch: Latest (Inverse3 support included)

**Clone and Study:**
```powershell
cd C:\Users\other\Documents\GitHub
git clone https://gitlab.com/Haply/public/chai3d-demos.git
cd chai3d-demos
```

**What to Look For:**
- Device initialization using `Inverse3` class
- `GetEndEffectorPosition()` method (not old Pantograph API)
- `SendEndEffectorForce()` method
- Proper HardwareAPI integration

---

### Option 3: Adapt Current Project (Advanced)

To make your current project work with Inverse3, you would need to:

1. **Replace Device Driver:**
   - Remove `Haply-API-cpp` (old Pantograph)
   - Add `HardwareAPI` for Inverse3
   - Update CMakeLists.txt to link against HardwareAPI

2. **Update Device Code:**
   ```cpp
   // OLD (Pantograph):
   Haply::HardwareAPI::Devices::Pantograph pantograph(&serial);
   
   // NEW (Inverse3):
   Haply::HardwareAPI::Devices::Inverse3 inverse3(&serial);
   auto response = inverse3.GetEndEffectorPosition(false);
   // position in response.position[0], [1], [2]
   ```

3. **Change CHAI3D Device Integration:**
   - Custom device class for Inverse3
   - Or use official Haply CHAI3D that already has this

---

## Recommended Path Forward

### For Inverse3 Development (Recommended)

**Step 1:** Download and test official releases
```powershell
# Get pre-built demos from:
https://develop.haply.co/releases/chai3d/3.3.5

# Extract and run to verify Inverse3 works
```

**Step 2:** Clone GitLab demos for source code
```powershell
git clone https://gitlab.com/Haply/public/chai3d-demos.git
```

**Step 3:** Study the demos
- See how they initialize Inverse3
- Understand the HardwareAPI integration
- Copy the pattern to your project

**Step 4:** Either:
- **Option A:** Use the official CHAI3D library from Haply (not GitHub fork)
- **Option B:** Adapt your project to use HardwareAPI directly

---

### For Pantograph Development

If you actually have the old 2-DOF Pantograph:
- ✅ Your current setup already works!
- The GitHub fork is designed for that device
- No changes needed

---

## Protocol Comparison

### Old Pantograph Protocol (Haply-API-cpp)

```cpp
// Initialize Pantograph
std::vector<std::string> ports = DeviceDetection::DetectPantographs();
SerialStream serial(ports[0].c_str());
Haply::HardwareAPI::Devices::Pantograph pantograph(&serial);

// Read position
auto response = pantograph.GetDevicePosition(false);
float x = response.position[0];
float y = response.position[1];
// Only 2 DOF (x, y)
```

### Inverse3 Protocol (HardwareAPI)

```cpp
// Initialize Inverse3
std::vector<std::string> ports = DeviceDetection::DetectInverse3s();
SerialStream serial(ports[0].c_str());
Haply::HardwareAPI::Devices::Inverse3 inverse3(&serial);

// Wake device
auto info = inverse3.DeviceWakeup();

// Read position
auto response = inverse3.GetEndEffectorPosition(false);
float x = response.position[0];
float y = response.position[1];
float z = response.position[2];
// Full 3 DOF (x, y, z)

// Send forces
Haply::HardwareAPI::Devices::Inverse3::EndEffectorForceRequest request;
request.force[0] = fx;
request.force[1] = fy;
request.force[2] = fz;
inverse3.EndEffectorForce(request);
```

---

## Resources

### Official Documentation

- **Haply Developer Hub:** https://develop.haply.co/
- **CHAI3D Releases:** https://develop.haply.co/releases/chai3d
- **Inverse3 Docs:** https://docs.haply.co/docs/developing-with-inverse3/
- **HardwareAPI C++ Docs:** https://docs.haply.co/hardwareAPI/cpp/

### Source Code

- **GitLab CHAI3D Demos:** https://gitlab.com/Haply/public/chai3d-demos
- **HardwareAPI Examples:** https://gitlab.com/Haply/public/hardware-api-cpp-examples

---

## Next Steps

### Immediate Action

1. **Verify your device:**
   - Do you have **Inverse3** or **old Pantograph**?
   - Check device label and model number

2. **If Inverse3:**
   - Download official CHAI3D from https://develop.haply.co/releases/chai3d/3.3.5
   - Test with pre-built demos
   - Study GitLab source code

3. **If Pantograph:**
   - Your current setup works!
   - No changes needed

---

## Current Project Status

### What Works ✅
- ✅ Builds successfully on Windows 11
- ✅ Runs in visual-only mode
- ✅ GLUT graphics rendering
- ✅ Graceful error handling
- ✅ All automation scripts
- ✅ Comprehensive documentation

### What Doesn't Work ❌ (with Inverse3)
- ❌ Position reading (always 0,0,0)
- ❌ Force output
- ❌ Haptic interaction

### Why
- GitHub CHAI3D fork uses wrong protocol for Inverse3
- Designed for old Pantograph device

---

## Migration Path (If Needed)

To properly support Inverse3 in this project:

### Option A: Replace CHAI3D Submodule

```powershell
# Remove old fork
git submodule deinit -f external/chai3d
git rm -f external/chai3d
rm -rf .git/modules/external/chai3d

# Use official Haply release instead
# (Download from develop.haply.co and extract to external/)
```

### Option B: Use Inverse SDK (Service-Based)

Instead of HardwareAPI direct serial:
- Use Haply Inverse SDK (>= 3.1.0)
- WebSocket protocol (language-agnostic)
- Works alongside Haply Hub
- No COM port conflicts

**SDK Integration:**
```cpp
// Connect to Inverse Service via WebSocket
// ws://localhost:10001
// Receive JSON with position/velocity
// Send JSON with force commands
```

See: https://docs.haply.co/inverseSDK/

---

## Conclusion

The current project successfully demonstrates:
- ✅ Complete CHAI3D integration on Windows
- ✅ Build system configuration
- ✅ Error handling and automation
- ✅ Professional documentation

For **Inverse3 haptic feedback**, you need:
- Official Haply CHAI3D (not GitHub fork)
- OR HardwareAPI integration
- OR Inverse SDK WebSocket approach

All paths are documented in official Haply resources at [develop.haply.co](https://develop.haply.co).

---

**Last Updated:** 04 February 2026  
**Status:** Documentation complete, device protocol clarified
