# CHAI3D + Haply Inverse3 — Full Project Setup Guide (Windows)

> **Target audience:** LLM coding agents (Cursor, Copilot, etc.) and human developers.  
> **Target OS:** Windows 11, x64 (tested on i9-14900HX / 32 GB RAM).  
> **Repo root:** `C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR`

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Prerequisites — Install Once](#2-prerequisites--install-once)
3. [Haply Hub & Device Driver Setup](#3-haply-hub--device-driver-setup)
4. [Project Directory Structure](#4-project-directory-structure)
5. [CHAI3D Source — Clone & Configure (Haply Fork)](#5-chai3d-source--clone--configure-haply-fork)
6. [Building CHAI3D Library (CMake + Visual Studio)](#6-building-chai3d-library-cmake--visual-studio)
7. [Building All CHAI3D GLUT Demos](#7-building-all-chai3d-glut-demos)
8. [CHAI3D Extension Modules (GEL, BULLET, ODE, OCULUS)](#8-chai3d-extension-modules-gel-bullet-ode-oculus)
9. [Haply CHAI3D Demos (Pre-built & Source)](#9-haply-chai3d-demos-pre-built--source)
10. [Haply HardwareAPI C++ (Direct Device Access)](#10-haply-hardwareapi-c-direct-device-access)
11. [Haply Inverse SDK (Service-based / WebSocket)](#11-haply-inverse-sdk-service-based--websocket)
12. [Running Demos with the Inverse3](#12-running-demos-with-the-inverse3)
13. [Creating Your Own CHAI3D + Inverse3 Application](#13-creating-your-own-chai3d--inverse3-application)
14. [Troubleshooting](#14-troubleshooting)
15. [Key Reference Links](#15-key-reference-links)

---

## 1. Architecture Overview

There are **three layers** you need to understand:

```
┌─────────────────────────────────────────────────┐
│              Your Application (C++)              │
├─────────────────────────────────────────────────┤
│     CHAI3D Framework (haptic + graphic engine)   │
│  ┌──────────────┐  ┌────────────────────────┐   │
│  │  Core Library │  │ Extension Modules      │   │
│  │  (libchai3d)  │  │ GEL / BULLET / ODE     │   │
│  └──────────────┘  └────────────────────────┘   │
├─────────────────────────────────────────────────┤
│  Device Interface (choose one):                  │
│  ┌──────────────────────┐ ┌───────────────────┐ │
│  │ HardwareAPI C++ (low  │ │ Inverse SDK 3.x   │ │
│  │ level, direct serial) │ │ (service+websocket)│ │
│  └──────────────────────┘ └───────────────────┘ │
├─────────────────────────────────────────────────┤
│        Haply Hub / Inverse Service (driver)       │
├─────────────────────────────────────────────────┤
│        Inverse3 Hardware (USB-C + 24V power)      │
└─────────────────────────────────────────────────┘
```

**CHAI3D** is the haptic rendering engine. It provides 3D scene graph, OpenGL rendering, force computation, and a device abstraction layer. The **Haply fork** of CHAI3D adds native Inverse3 support via a CMake flag.

**HardwareAPI** is a low-level C++ static library from Haply for direct serial communication with the Inverse3 at up to 4 kHz.

**Inverse SDK (≥ 3.1.0)** is the newer service-based architecture where a background service (`haply-inverse-service`) handles device discovery, pairing, and communication. Applications connect via WebSocket (language-agnostic) or the C++/Unity SDK.

---

## 2. Prerequisites — Install Once

### 2.1 Visual Studio 2022

Download from: https://visualstudio.microsoft.com/downloads/

During installation, select the workload:
- **"Desktop development with C++"**

This installs MSVC v143 toolset, Windows SDK, and CMake integration.

If you already have VS2022 installed, open **Visual Studio Installer → Modify** and ensure the C++ workload is checked.

> **Note:** CHAI3D ships with solution files for VS2012/2013/2015, but the CMake build works perfectly with VS2022 and the v143 toolset. This is the recommended path.

### 2.2 CMake (≥ 3.0)

Visual Studio 2022 bundles CMake, but it's recommended to install the standalone version for command-line use:

```powershell
# Option A: winget (recommended)
winget install Kitware.CMake

# Option B: Download from https://cmake.org/download/
# Choose "Windows x64 Installer" (.msi), check "Add to PATH"
```

Verify:
```powershell
cmake --version
# Should be 3.x or 4.x
```

### 2.3 Git

```powershell
winget install Git.Git
```

After install, restart your terminal and verify:
```powershell
git --version
```

### 2.4 OpenGL Drivers

CHAI3D uses OpenGL for rendering. Your Intel UHD Graphics should work out of the box with Windows 11, but if you see black screens or crashes, update GPU drivers from:
- Intel: https://www.intel.com/content/www/us/en/download-center/home.html
- If you have a discrete NVIDIA GPU: https://www.nvidia.com/Download/index.aspx

---

## 3. Haply Hub & Device Driver Setup

The Haply Hub is the essential driver/manager for the Inverse3.

### 3.1 Download & Install

1. Go to: https://develop.haply.co/releases/manager/latest
2. Download the Windows installer and run it.
3. The installer places the **Haply Hub** application and the **Inverse Service** (background daemon).

### 3.2 Connect Hardware

1. Mount the Inverse3 to the desk clamp, ensure it's perpendicular to the ground.
2. Unfold the arms (reverse the fold procedure).
3. Connect the USB-C cable **directly** to your computer (avoid dongles/hubs for best performance).
4. Wait for the status light: **red → purple** (indicates USB connection established).
5. Plug in the 24V power supply barrel jack.
6. Status light should now be steady.

### 3.3 Calibration

Touch the protruding magnet on the gimbal to the calibration point on the device body. Status light turns **white** when calibration is complete and the device is ready.

### 3.4 Verify in Haply Hub

Open the Haply Hub application. You should see your Inverse3 listed with its device ID. This confirms the Inverse Service is running and communicating.

> **Important:** The Inverse Service must be version **≥ 3.1.0** for compatibility with the latest CHAI3D demos. The Haply Hub will show the service version.

---

## 4. Project Directory Structure

Set up the following structure inside your repo:

```
AIMLAB-Unity-Chai3D-VR/
├── .gitattributes
├── .gitignore
├── README.md
├── docs/                          # Documentation
│   └── SETUP_INSTRUCTIONS.md      # This file
├── external/                      # Third-party dependencies (git submodules or downloads)
│   ├── chai3d/                    # Haply fork of CHAI3D (git submodule)
│   └── HardwareAPI/               # Haply HardwareAPI C++ (from develop.haply.co)
├── chai3d-demos/                  # Haply's official CHAI3D demos (from GitLab)
├── src/                           # Your own application source code
│   ├── CMakeLists.txt
│   └── main.cpp
├── resources/                     # 3D models, textures, sounds for your app
├── bin/                           # Built executables end up here
│   └── resources/                 # Runtime resources copied here
├── build/                         # CMake build directory (gitignored)
└── CMakeLists.txt                 # Top-level CMake for your project
```

### 4.1 Create the structure

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

mkdir docs, external, src, resources, bin
```

### 4.2 Update .gitignore

Append the following to `.gitignore`:

```gitignore
# Build artifacts
build/
bin/*.exe
bin/*.pdb
bin/*.dll
*.obj
*.lib
*.exp

# IDE
.vs/
*.suo
*.user
*.sln.docstates
out/

# CMake
CMakeFiles/
CMakeCache.txt
cmake_install.cmake

# OS
Thumbs.db
Desktop.ini
.DS_Store
```

---

## 5. CHAI3D Source — Clone & Configure (Haply Fork)

The Haply fork of CHAI3D is the version you want. It adds Inverse3 device support via the Haply-API-cpp submodule.

### 5.1 Add as Git Submodule

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

# Add Haply's CHAI3D fork as a submodule
git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d

# Checkout the correct branch (haply-api-cpp)
cd external/chai3d
git checkout haply-api-cpp

# Initialize the Haply-API-cpp submodule inside CHAI3D
git submodule update --init --recursive

cd ../..
```

> **What this gets you:** The full CHAI3D framework source, 31 GLUT example programs, 5 extension modules (BULLET, GEL, ODE, OCULUS, V-REP), project templates, and Haply device integration.

### 5.2 Verify the Submodule

```powershell
ls external\chai3d
```

You should see: `CMakeLists.txt`, `src/`, `examples/`, `modules/`, `external/`, `bin/`, `templates/`, etc.

```powershell
ls external\chai3d\external
```

You should see the `Haply-API-cpp/` directory (the Haply device driver submodule).

---

## 6. Building CHAI3D Library (CMake + Visual Studio)

### 6.1 Method A: CMake Command Line (Recommended for Cursor/Agents)

Open **Developer PowerShell for VS 2022** (or any terminal with MSVC in PATH):

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR\external\chai3d

# Create out-of-source build directory
mkdir build
cd build

# Configure with CMake — enable Haply device support
cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON

# Build the core library in Release mode
cmake --build . --config Release --target chai3d

# Build ALL targets (library + all examples + modules) — takes several minutes
cmake --build . --config Release
```

**Key CMake flags:**
- `-G "Visual Studio 17 2022"` — Generates VS2022 solution files
- `-A x64` — 64-bit target (required)
- `-DENABLE_HAPLY_DEVICES=ON` — Enables Haply Inverse3 support (activates GPLv3 license for the Haply component)

### 6.2 Method B: Open in Visual Studio 2022 GUI

1. Open Visual Studio 2022.
2. **File → Open → CMake...** → navigate to `external\chai3d\CMakeLists.txt`.
3. In the CMake Settings editor that appears, add the cache variable:
   - Name: `ENABLE_HAPLY_DEVICES`, Type: `BOOL`, Value: `ON`
4. Select configuration **x64-Release**.
5. **Build → Build All** (Ctrl+Shift+B).

### 6.3 Method C: Use the Pre-made Visual Studio Solution

CHAI3D also ships `.sln` files for older VS versions. While VS2022 can upgrade these:

```powershell
# Open e.g. the VS2015 solution (VS2022 will auto-upgrade)
start external\chai3d\CHAI3D-VS2015.sln
```

> **Warning:** The pre-made `.sln` files do NOT include the Haply device option. You must use the CMake path (Method A or B) for Haply support.

### 6.4 Build Output

After building, the compiled library and example executables appear in:

```
external/chai3d/build/Release/          # chai3d.lib (static library)
external/chai3d/bin/win-x64/            # Example executables (if using pre-made .sln)
```

Or if using CMake out-of-source build:
```
external/chai3d/build/bin/Release/      # All example executables
```

---

## 7. Building All CHAI3D GLUT Demos

The CMake build from Step 6 automatically builds all 31 GLUT demos. Here is the complete list:

| # | Demo | Description |
|---|------|-------------|
| 01 | `01-mydevice` | Basic haptic device test — read position, send forces |
| 02 | `02-multi-devices` | Using multiple haptic devices simultaneously |
| 03 | `03-analytics` | Performance analytics and haptic rate monitoring |
| 04 | `04-shapes` | Basic 3D shapes with haptic interaction |
| 05 | `05-fonts` | 3D text rendering with font support |
| 06 | `06-images` | Image/bitmap display in 3D scene |
| 07 | `07-mouse-select` | Object selection via mouse click |
| 08 | `08-shaders` | GLSL shader effects on haptic objects |
| 09 | `09-magnets` | Magnetic force field effects |
| 10 | `10-oring` | O-ring simulation (deformable ring on pegs) |
| 11 | `11-effects` | Surface haptic effects (vibration, stick-slip, viscosity) |
| 12 | `12-polygons` | Polygon mesh rendering and haptic interaction |
| 13 | `13-primitives` | Haptic interaction with geometric primitives |
| 14 | `14-textures` | Texture mapping on 3D objects |
| 15 | `15-paint` | 3D painting with haptic brush |
| 16 | `16-friction` | Friction effects on surfaces |
| 17 | `17-shading` | Advanced material shading |
| 18 | `18-endoscope` | Endoscope camera simulation |
| 19 | `19-space` | Navigating 3D space with haptic device |
| 20 | `20-map` | Heightmap terrain with force feedback |
| 21 | `21-object` | Loading and interacting with 3D model files (.obj, .3ds) |
| 22 | `22-chrome` | Chrome/reflective material rendering |
| 23 | `23-tooth` | Dental simulation (tooth drilling) |
| 24 | `24-turntable` | Turntable/rotation scene |
| 25 | `25-sounds` | Spatial audio with haptic interaction |
| 26 | `26-video` | Video texture playback |
| 27 | `27-multiframes` | Multiple viewports / rendering frames |
| 28 | `28-voxel-basic` | Basic volumetric (voxel) rendering |
| 29 | `29-voxel-isosurface` | Isosurface extraction from voxel data |
| 30 | `30-voxel-colormap` | Colormapped voxel rendering |
| 31 | `31-pointcloud` | Point cloud visualization with haptic probing |

### Running a Demo

```powershell
cd external\chai3d\build\bin\Release
.\13-primitives.exe
```

Each demo opens a GLUT window. Press `f` for fullscreen, `ESC` to quit. The haptic device (Inverse3) will be auto-detected if the Haply Hub / Inverse Service is running.

---

## 8. CHAI3D Extension Modules (GEL, BULLET, ODE, OCULUS)

Each module lives in `external/chai3d/modules/` and has its own CMakeLists.txt, examples, and documentation.

### 8.1 Module Overview

| Module | Description | Demos |
|--------|-------------|-------|
| **GEL** | Deformable body simulation (gel, membranes, soft tissue) | `50-GEL-membrane`, `51-GEL-duck` |
| **BULLET** | Rigid body physics via Bullet Physics engine | `40-ODE-cube`, `41-ODE-tool`, etc. |
| **ODE** | Rigid body physics via Open Dynamics Engine | `40-ODE-cube`, `41-ODE-tool`, `42-ODE-mesh` |
| **OCULUS** | Oculus Rift VR headset support (VS2015 era) | `60-OculusRift` |
| **V-REP** | CoppeliaSim (formerly V-REP) integration | Plugin for CoppeliaSim |

### 8.2 Building Modules

Each module must be built **after** the core CHAI3D library:

```powershell
# Build GEL module (deformable bodies — the gel membrane demo uses this)
cd external\chai3d\modules\GEL
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Build ODE module (rigid body physics)
cd ..\..\ODE
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Build BULLET module
cd ..\..\BULLET
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

> **Note:** Module CMake builds expect the core CHAI3D library to be already built. CMake uses its registry to find the CHAI3D config automatically.

### 8.3 GEL Module (Membrane Demo)

The **GEL module** is particularly important because the **gel membrane demo** (puncturing a deformable membrane) is one of the flagship Haply Inverse3 demos. After building:

```powershell
cd external\chai3d\modules\GEL\build\bin\Release
.\50-GEL-membrane.exe
```

---

## 9. Haply CHAI3D Demos (Pre-built & Source)

Haply provides a **separate demo package** built specifically for the Inverse3.

### 9.1 Pre-built Demo Bundle (Quickest Way to Test)

Download the pre-built demo bundle directly:

```powershell
# Download from Haply's CDN
Invoke-WebRequest -Uri "https://cdn.haply.co/r/Demo-Bundle/latest/Demo-bundle.zip" -OutFile "$env:USERPROFILE\Downloads\Demo-bundle.zip"

# Extract
Expand-Archive "$env:USERPROFILE\Downloads\Demo-bundle.zip" -DestinationPath "$env:USERPROFILE\Downloads\Demo-bundle"
```

Run any `.exe` in the extracted folder. The demos auto-detect the Inverse3 through the Inverse Service.

> **Requirements:** Inverse Service version ≥ 3.1.0 must be running (installed with Haply Hub). If the service is not running, demos will fall back to HardwareAPI direct connection.

### 9.2 Haply CHAI3D Demos Source (GitLab)

To build the demos from source and customize them:

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

# Clone the Haply CHAI3D Demos repo
git clone https://gitlab.com/Haply/public/chai3d-demos.git chai3d-demos

cd chai3d-demos
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### 9.3 Haply's Pre-built CHAI3D Release (v3.3.2)

Haply also distributes a custom CHAI3D build with Inverse3 support baked in:

- **Download page:** https://develop.haply.co/releases/chai3d/3.3.2
- **Direct download (Windows):** https://cdn.haply.co/r/Chai3D/3.3.2/windows/Chai3D.3.3.2.zip

This zip contains pre-compiled CHAI3D libraries and demo executables for Windows. Extract and run.

---

## 10. Haply HardwareAPI C++ (Direct Device Access)

The HardwareAPI is a **static C++ library** for direct, low-level communication with the Inverse3 over serial (USB). This bypasses the Inverse Service and gives you maximum control and lowest latency.

### 10.1 Download

Go to: https://develop.haply.co/releases  
Look for **"HardwareAPI C++"** and download the Windows build.

Alternatively, check the GitLab examples repo:  
https://gitlab.com/Haply/public/hardware-api-cpp-examples

### 10.2 Integration

Place the HardwareAPI files in your project:

```
external/
  HardwareAPI/
    include/
      HardwareAPI.h
    lib/
      win-x64/
        HardwareAPI.lib
```

### 10.3 Minimal Example — Read Position

```cpp
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include "HardwareAPI.h"

int main() {
    // Auto-detect Inverse3 devices
    std::vector<std::string> ports =
        Haply::HardwareAPI::Devices::DeviceDetection::DetectInverse3s();

    if (ports.empty()) {
        std::cerr << "No Inverse3 found. Check USB connection." << std::endl;
        return 1;
    }

    // Open serial connection to first device
    Haply::HardwareAPI::IO::SerialStream serial(ports[0].c_str());
    Haply::HardwareAPI::Devices::Inverse3 inverse3(&serial);

    // Wake up the device
    auto info = inverse3.DeviceWakeup();
    std::cout << "Connected to Inverse3, Device ID: " << info.device_id << std::endl;

    // Read position loop
    while (true) {
        auto response = inverse3.GetEndEffectorPosition(false);
        printf("Position: x=%.4f y=%.4f z=%.4f\n",
               response.position[0], response.position[1], response.position[2]);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    return 0;
}
```

### 10.4 Minimal Example — Force Feedback

```cpp
// In your haptic loop:
Haply::HardwareAPI::Devices::Inverse3::EndEffectorForceRequest request;
request.force[0] = 0.0f;  // X force in Newtons
request.force[1] = 0.0f;  // Y force
request.force[2] = 0.0f;  // Z force

auto response = inverse3.EndEffectorForce(request);
// response.position[] and response.velocity[] are updated
```

---

## 11. Haply Inverse SDK (Service-based / WebSocket)

The Inverse SDK (≥ 3.1.0) is the **recommended modern approach** for new projects. It uses a background service that handles device management.

### 11.1 Install

Download from: https://develop.haply.co/releases  
Look for **"Inverse Installer"** or it's bundled with the Haply Hub.

The installer sets up:
- `haply-inverse-service` (background service, auto-starts)
- WebSocket endpoint at `ws://localhost:10001`
- REST API for device discovery at `http://localhost:10000`

### 11.2 WebSocket Protocol (Language-Agnostic)

Any language can connect via WebSocket. The service streams JSON payloads:

```json
{
  "inverse3": [
    {
      "device_id": "INV3-XXXX",
      "position": { "x": 0.0, "y": 0.0, "z": 0.0 },
      "velocity": { "x": 0.0, "y": 0.0, "z": 0.0 },
      "config": { "handedness": "right" }
    }
  ],
  "wireless_verse_grip": [...]
}
```

Send forces back as JSON:

```json
{
  "device_id": "INV3-XXXX",
  "force": { "x": 0.0, "y": 0.0, "z": 1.0 }
}
```

### 11.3 Unity Integration

For the Unity side of this project, Haply provides a Unity package:

1. In Unity Editor: **Edit → Project Settings → Package Manager**
2. Add Scoped Registry:
   - Name: `Haply`
   - URL: `https://registry.npmjs.org`
   - Scope: `co.haply`
3. **Window → Package Manager → Add by name:** `co.haply.inverse`
4. Create a **Haptic Rig**: **GameObject → Haply Inverse → Haptic Rig (one hand)**

> Requires Inverse SDK ≥ 3.1.0 and Unity 2021.3+.

---

## 12. Running Demos with the Inverse3

### Pre-flight Checklist

1. ☐ Inverse3 mounted, arms unfolded
2. ☐ USB-C connected directly to PC (status light: red → purple)
3. ☐ 24V power supply connected
4. ☐ Calibrated (magnet touched to calibration point, light turns white)
5. ☐ Haply Hub running and showing the device
6. ☐ Inverse Service ≥ 3.1.0 active

### Running CHAI3D Examples

```powershell
cd external\chai3d\build\bin\Release

# Basic device test
.\01-mydevice.exe

# Shapes demo
.\04-shapes.exe

# Primitives (popular starting demo)
.\13-primitives.exe

# Surface effects
.\11-effects.exe

# Tooth drilling simulation
.\23-tooth.exe
```

### Running Haply Pre-built Demos

```powershell
cd $env:USERPROFILE\Downloads\Demo-bundle\*

# Run whatever executables are in the bundle
Get-ChildItem *.exe | ForEach-Object { Write-Host $_.Name }
```

### Controls (Common Across Demos)

- **`f`** — Toggle fullscreen
- **`ESC`** — Quit
- **`q`** — Quit (some demos)
- **Mouse drag** — Rotate camera view
- **Scroll** — Zoom
- **Inverse3 cursor** — Interact with haptic objects

---

## 13. Creating Your Own CHAI3D + Inverse3 Application

### 13.1 Top-Level CMakeLists.txt

Create `C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR\CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)
project(AIMLAB-Haptics VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── CHAI3D (the submodule in external/chai3d) ─────────────────────
# CHAI3D must be built first (see setup instructions Step 6).
# After building, CMake registers it so find_package works.
find_package(CHAI3D REQUIRED)

# Include CHAI3D headers
include_directories(${CHAI3D_INCLUDE_DIRS})

# ─── FreeGLUT (bundled with CHAI3D on Windows) ─────────────────────
if(WIN32)
    # CHAI3D bundles freeglut in its external/ directory
    set(FREEGLUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/chai3d/external/freeglut")
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

# ─── Your Application ──────────────────────────────────────────────
add_executable(aimlab-haptics src/main.cpp)
target_link_libraries(aimlab-haptics ${CHAI3D_LIBRARIES} ${GLUT_LIBRARIES})

# ─── Copy resources to build output ────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
```

### 13.2 Starter main.cpp

Create `src/main.cpp`:

```cpp
//===========================================================================
// AIMLAB Haptics — Starter Application
// Based on CHAI3D framework with Haply Inverse3 support
//===========================================================================

#include "chai3d.h"
#include <GLUT/glut.h>    // macOS
// #include <GL/glut.h>   // Linux
// On Windows, CHAI3D's CMake handles the GLUT include path

using namespace chai3d;
using namespace std;

// ─── Global Variables ────────────────────────────────────────────────
cWorld* world;
cCamera* camera;
cDirectionalLight* light;
cHapticDeviceHandler* handler;
cGenericHapticDevicePtr hapticDevice;
cToolCursor* tool;
cSphere* sphere;

int windowW = 1024;
int windowH = 768;
bool simulationRunning = false;
bool simulationFinished = false;

// ─── Haptic Thread ──────────────────────────────────────────────────
cThread* hapticThread;

void updateHaptics() {
    simulationRunning = true;
    while (simulationRunning) {
        world->computeGlobalPositions(true);
        tool->updateFromDevice();
        tool->computeInteractionForces();
        tool->applyToDevice();
    }
    simulationFinished = true;
}

// ─── Graphics Callbacks ─────────────────────────────────────────────
void updateGraphics() {
    camera->renderView(windowW, windowH);
    glutSwapBuffers();
    if (simulationRunning) glutPostRedisplay();
}

void resizeWindow(int w, int h) {
    windowW = w;
    windowH = h;
}

void keySelect(unsigned char key, int x, int y) {
    if (key == 27 || key == 'q') {  // ESC or 'q'
        simulationRunning = false;
        while (!simulationFinished) cSleepMs(100);
        exit(0);
    }
    if (key == 'f') glutFullScreen();
}

// ─── Main ───────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    // ── GLUT init ──
    glutInit(&argc, argv);
    glutInitWindowSize(windowW, windowH);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("AIMLAB - Haptic Environment");
    glutDisplayFunc(updateGraphics);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(keySelect);

    // ── World ──
    world = new cWorld();
    world->m_backgroundColor.setBlack();

    // ── Camera ──
    camera = new cCamera(world);
    world->addChild(camera);
    camera->set(cVector3d(0.5, 0.0, 0.3),   // position
                cVector3d(0.0, 0.0, 0.0),    // lookat
                cVector3d(0.0, 0.0, 1.0));   // up

    // ── Light ──
    light = new cDirectionalLight(world);
    world->addChild(light);
    light->setDir(-1.0, -1.0, -1.0);

    // ── Haptic Device (auto-detects Inverse3) ──
    handler = new cHapticDeviceHandler();
    handler->getDevice(hapticDevice, 0);
    hapticDevice->open();
    hapticDevice->calibrate();

    // ── Tool Cursor ──
    tool = new cToolCursor(world);
    world->addChild(tool);
    tool->setHapticDevice(hapticDevice);
    tool->setRadius(0.005);       // cursor sphere radius
    tool->setWorkspaceRadius(0.1);
    tool->start();

    // ── Scene Object: a sphere to touch ──
    sphere = new cSphere(0.03);
    world->addChild(sphere);
    sphere->setLocalPos(0.0, 0.0, 0.0);
    sphere->m_material->setStiffness(1000.0);
    sphere->m_material->setStaticFriction(0.3);
    sphere->m_material->setDynamicFriction(0.2);
    sphere->setHapticEnabled(true);

    // ── Start Haptic Thread ──
    hapticThread = new cThread();
    hapticThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    // ── Start Graphics Loop ──
    glutMainLoop();

    return 0;
}
```

### 13.3 Build Your Application

```powershell
cd C:\Users\other\Documents\GitHub\AIMLAB-Unity-Chai3D-VR

mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## 14. Troubleshooting

### Device Not Detected

| Symptom | Fix |
|---------|-----|
| Status light stays red | Check USB cable, try a different port, use the included cable |
| Haply Hub doesn't show device | Reinstall Haply Hub, check Windows Device Manager for COM port |
| Demo says "No device found" | Ensure Inverse Service ≥ 3.1.0 is running; try restarting the service |
| Demo opens but no haptic feedback | Calibrate the device (magnet to calibration point), check 24V power |

### Build Errors

| Error | Fix |
|-------|-----|
| `ENABLE_HAPLY_DEVICES` has no effect | Ensure you ran `git submodule update --init --recursive` in the chai3d directory |
| CMake can't find Visual Studio | Open "Developer PowerShell for VS 2022" or pass `-G "Visual Studio 17 2022"` explicitly |
| Linker errors (unresolved externals) | Ensure you're building x64, not x86. Use `-A x64` in CMake |
| `find_package(CHAI3D)` fails | Build CHAI3D first (Step 6). CMake registers it in its package registry during build |
| FreeGLUT not found | On Windows, CHAI3D bundles FreeGLUT in `external/freeglut/`. The CMake build handles this automatically |
| OpenGL errors / black screen | Update your GPU drivers. Intel UHD should work, but check for latest driver |

### Performance

- **Always build in Release mode** for haptic applications. Debug mode introduces frame drops.
- Connect USB-C directly to the PC — avoid hubs and dongles.
- Haptic loop should run at ≥ 1 kHz. Use `cThread` with `CTHREAD_PRIORITY_HAPTICS`.
- Keep haptic thread computation minimal — no file I/O, no memory allocation in the loop.

---

## 15. Key Reference Links

### Haply

| Resource | URL |
|----------|-----|
| Haply Documentation Hub | https://docs.haply.co/ |
| Getting Started (Inverse3) | https://docs.haply.co/docs/quick-start/ |
| Developing with Inverse3 | https://docs.haply.co/docs/developing-with-inverse3/ |
| Inverse SDK Docs | https://docs.haply.co/inverseSDK/ |
| HardwareAPI C++ Docs | https://docs.haply.co/hardwareAPI/cpp/ |
| Developer Hub / Downloads | https://develop.haply.co/releases |
| CHAI3D v3.3.2 (Haply build) | https://develop.haply.co/releases/chai3d/3.3.2 |
| Demo Bundle (pre-built) | https://cdn.haply.co/r/Demo-Bundle/latest/Demo-bundle.zip |
| CHAI3D Demos Source (GitLab) | https://gitlab.com/Haply/public/chai3d-demos |
| HardwareAPI C++ Examples (GitLab) | https://gitlab.com/Haply/public/hardware-api-cpp-examples |
| Haply Forum | https://forum.haply.co/ |

### CHAI3D

| Resource | URL |
|----------|-----|
| CHAI3D Homepage | https://www.chai3d.org/ |
| Official Releases | https://www.chai3d.org/download/releases |
| API Documentation | https://www.chai3d.org/download/doc/html/wrapper-overview.html |
| Installation Guide | https://www.chai3d.org/download/doc/html/chapter2-installation.html |
| Project Creation Guide | https://www.chai3d.org/download/doc/html/chapter4-creation.html |
| GitHub (Official) | https://github.com/chai3d/chai3d |
| GitHub (Haply Fork) | https://github.com/HaplyHaptics/chai3d |
| Forum | https://www.chai3d.org/forum/index |

### Tools

| Resource | URL |
|----------|-----|
| Visual Studio 2022 | https://visualstudio.microsoft.com/downloads/ |
| CMake Downloads | https://cmake.org/download/ |
| Git for Windows | https://git-scm.com/download/win |

---

## License Notes

- **CHAI3D core** is BSD 3-Clause licensed.
- **Enabling Haply device support** (`ENABLE_HAPLY_DEVICES=ON`) switches the Haply component to **GPLv3**.
- **Haply HardwareAPI** is licensed under GPLv3.
- **Haply Inverse SDK** — check the Haply license terms on their developer portal.

If you publish work using CHAI3D with Haply devices, cite both the CHAI3D paper (Conti et al., 2003) and the Haply CHAI3D paper (Frisson et al., HAID 2019). See the README in the Haply CHAI3D fork for full BibTeX entries.
