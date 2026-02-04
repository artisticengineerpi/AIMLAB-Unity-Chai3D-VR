/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - Development Guide
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Developer guide for extending and customizing the AIMLAB haptic
 *   application. Includes code examples, API references, and best practices.
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial development guide
 * 
 ****************************************************************************/

# AIMLAB Haptics - Development Guide

## Table of Contents

1. [Adding New Scene Objects](#adding-new-scene-objects)
2. [Custom Force Effects](#custom-force-effects)
3. [Loading 3D Models](#loading-3d-models)
4. [Haptic Material Properties](#haptic-material-properties)
5. [Multi-Device Support](#multi-device-support)
6. [Performance Optimization](#performance-optimization)
7. [Debugging Tips](#debugging-tips)

---

## Adding New Scene Objects

### Creating a Basic Shape

```cpp
// Create a box
cMesh* box = new cMesh();
cCreateBox(box, 0.05, 0.05, 0.05);  // 50mm cube
world->addChild(box);
box->setLocalPos(0.1, 0.0, 0.0);
box->m_material->setStiffness(500.0);
box->setHapticEnabled(true);

// Create a cylinder
cMesh* cylinder = new cMesh();
cCreateCylinder(cylinder, 0.02, 0.1);  // radius 20mm, height 100mm
world->addChild(cylinder);
cylinder->setLocalPos(-0.1, 0.0, 0.0);
```

### Adding Textures

```cpp
// Load texture
cTexture2dPtr texture = cTexture2d::create();
bool success = texture->loadFromFile("../resources/texture.png");

// Apply to object
sphere->setTexture(texture);
sphere->setUseTexture(true);
```

---

## Custom Force Effects

### Spring Force to Origin

```cpp
void updateHaptics() {
    while (simulationRunning) {
        // Get device position
        cVector3d devicePos;
        hapticDevice->getPosition(devicePos);

        // Compute spring force toward origin
        cVector3d force = -500.0 * devicePos;  // 500 N/m stiffness

        // Apply force to device
        hapticDevice->setForce(force);
    }
}
```

### Viscous Damping

```cpp
// Get device velocity
cVector3d velocity;
hapticDevice->getLinearVelocity(velocity);

// Compute damping force
double dampingCoeff = 10.0;  // N·s/m
cVector3d dampingForce = -dampingCoeff * velocity;

hapticDevice->setForce(dampingForce);
```

### Magnetic Attraction

```cpp
// Attract to a point
cVector3d attractorPos(0.0, 0.0, 0.1);
cVector3d devicePos;
hapticDevice->getPosition(devicePos);

cVector3d direction = attractorPos - devicePos;
double distance = direction.length();

if (distance > 0.001) {  // Avoid singularity
    direction.normalize();
    double magnetStrength = 50.0;  // N
    double forceMag = magnetStrength / (distance * distance);  // Inverse square
    cVector3d force = forceMag * direction;
    
    // Clamp to device max force
    double maxForce = info.m_maxLinearForce;
    if (force.length() > maxForce) {
        force.normalize();
        force *= maxForce;
    }
    
    hapticDevice->setForce(force);
}
```

---

## Loading 3D Models

### Load OBJ File

```cpp
#include "chai3d.h"

// Create mesh object
cMesh* model = new cMesh();

// Load from file
bool success = model->loadFromFile("../resources/bunny.obj");

if (success) {
    world->addChild(model);
    model->setLocalPos(0.0, 0.0, 0.0);
    model->scale(0.1);  // Scale to appropriate size
    
    // Compute collision detection tree
    model->createAABBCollisionDetector(toolRadius);
    
    // Configure haptics
    model->m_material->setStiffness(1000.0);
    model->setHapticEnabled(true);
    
    // Compute normals for smooth shading
    model->computeAllNormals();
} else {
    cerr << "Failed to load model!" << endl;
}
```

### Supported Formats

- `.obj` - Wavefront OBJ (most common)
- `.3ds` - 3D Studio Max
- `.stl` - Stereolithography

---

## Haptic Material Properties

### Material Property Reference

```cpp
// Stiffness: Surface hardness (N/m)
// Range: 0 - 3000 for Inverse3
sphere->m_material->setStiffness(1000.0);

// Static Friction: Resistance when starting to slide
// Range: 0.0 - 1.0
sphere->m_material->setStaticFriction(0.5);

// Dynamic Friction: Resistance while sliding
// Range: 0.0 - 1.0 (usually < static friction)
sphere->m_material->setDynamicFriction(0.3);

// Viscosity: Fluid resistance (N·s/m)
// Range: 0.0 - 100.0
sphere->m_material->setViscosity(5.0);

// Vibration Effect
sphere->m_material->setVibrationFrequency(200.0);  // Hz
sphere->m_material->setVibrationAmplitude(0.5);    // N
sphere->m_material->setUseHapticShading(true);

// Magnetic Effect
sphere->m_material->setMagnetMaxForce(5.0);        // N
sphere->m_material->setMagnetMaxDistance(0.05);    // m
sphere->m_material->setUseMaterial(true);
```

### Material Presets

```cpp
// Soft rubber
material->setStiffness(200.0);
material->setStaticFriction(0.8);
material->setDynamicFriction(0.6);

// Hard metal
material->setStiffness(3000.0);
material->setStaticFriction(0.2);
material->setDynamicFriction(0.15);

// Gel/Membrane
material->setStiffness(100.0);
material->setViscosity(20.0);
```

---

## Multi-Device Support

### Detecting Multiple Devices

```cpp
cHapticDeviceHandler* handler = new cHapticDeviceHandler();

// Get number of available devices
int numDevices = handler->getNumDevices();

cout << "Found " << numDevices << " haptic device(s)" << endl;

// Create tools for each device
for (int i = 0; i < numDevices; i++) {
    cGenericHapticDevicePtr device;
    handler->getDevice(device, i);
    
    if (device != nullptr) {
        device->open();
        device->calibrate();
        
        cToolCursor* tool = new cToolCursor(world);
        tool->setHapticDevice(device);
        tool->start();
        world->addChild(tool);
    }
}
```

---

## Performance Optimization

### Haptic Thread Best Practices

**DO:**
- Keep haptic loop computation minimal
- Use pre-computed values when possible
- Avoid memory allocation in the loop
- Use Release builds (10x faster than Debug)
- Maintain >= 1 kHz update rate

**DON'T:**
- Perform file I/O in haptic thread
- Allocate/deallocate memory
- Use blocking operations
- Print to console in the loop (slows down significantly)

### Monitoring Performance

```cpp
// In haptic thread - add frequency counter
cFrequencyCounter frequencyCounter;

void updateHaptics() {
    while (simulationRunning) {
        frequencyCounter.signal(1);
        
        // ... haptic loop code ...
        
        // Print frequency occasionally (not every frame!)
        static int counter = 0;
        if (++counter % 1000 == 0) {
            cout << "Haptic rate: " << frequencyCounter.getFrequency() 
                 << " Hz" << endl;
        }
    }
}
```

### Collision Detection Optimization

```cpp
// For simple objects: use analytical collision detection (fast)
sphere->setHapticEnabled(true);  // Spheres use analytical collision

// For complex meshes: use AABB tree
model->createAABBCollisionDetector(toolRadius);

// For very complex meshes: lower triangle count
model->loadFromFile("complex_model.obj");
model->reduceNumberOfTriangles(5000);  // Decimate to 5000 triangles
model->createAABBCollisionDetector(toolRadius);
```

---

## Debugging Tips

### Enable Console Output

The `main.cpp` already includes console output. To add more debugging:

```cpp
// In haptic thread
cVector3d pos;
hapticDevice->getPosition(pos);
printf("Position: [%.3f, %.3f, %.3f]\n", pos.x(), pos.y(), pos.z());

// Force output
cVector3d force;
hapticDevice->getForce(force);
printf("Force: [%.3f, %.3f, %.3f] N\n", force.x(), force.y(), force.z());
```

### Visualize Collision Tree

```cpp
// Show collision detection tree (for debugging)
model->setShowBoundaryBox(true);
model->setShowCollisionDetector(true);
```

### Check Device Specifications

```cpp
cHapticDeviceInfo info = hapticDevice->getSpecifications();

cout << "Device: " << info.m_modelName << endl;
cout << "Max Force: " << info.m_maxLinearForce << " N" << endl;
cout << "Max Stiffness: " << info.m_maxLinearStiffness << " N/m" << endl;
cout << "Workspace Radius: " << info.m_workspaceRadius << " m" << endl;
```

---

## Code Examples Repository

Additional example code is available in:
- `external/chai3d/examples/` - 31 GLUT demos
- CHAI3D documentation: https://www.chai3d.org/download/doc/html/

### Recommended Examples to Study

| Example | What it Demonstrates |
|---------|---------------------|
| `01-mydevice` | Basic device connection and force output |
| `04-shapes` | Multiple geometric primitives |
| `11-effects` | Surface effects (vibration, friction, viscosity) |
| `13-primitives` | Sphere, box, cylinder interaction |
| `21-object` | Loading and rendering 3D models |
| `23-tooth` | Complex simulation (dental drilling) |

---

## Project Extension Ideas

### 1. VR Integration
- Add Unity interop via shared memory or network
- Send haptic feedback from Unity physics to CHAI3D
- Synchronize VR camera with CHAI3D viewport

### 2. Advanced Haptics
- Implement custom force algorithms
- Add texture-based force variation
- Create haptic widgets (buttons, sliders)

### 3. Multi-Modal Feedback
- Integrate spatial audio (see demo `25-sounds`)
- Add visual effects synchronized with haptic events
- Implement haptic recording and playback

### 4. Research Tools
- Data logging (position, force, time-series)
- Performance metrics collection
- User study framework

---

## API Quick Reference

### Common CHAI3D Classes

- `cWorld` - Scene graph root
- `cCamera` - Viewport and rendering
- `cLight` - Lighting (directional, point, spot)
- `cMesh` - Triangle mesh objects
- `cMultiMesh` - Multi-part meshes
- `cShapeSphere`, `cShapeBox`, `cShapeCylinder` - Primitives
- `cMaterial` - Material properties
- `cTexture2d` - Texture mapping
- `cHapticDeviceHandler` - Device manager
- `cGenericHapticDevice` - Device interface
- `cToolCursor` - Haptic interaction tool
- `cThread` - Thread management
- `cPrecisionClock` - High-resolution timing

### Haply Inverse3 Specifications

- **Workspace:** 160mm × 100mm × 130mm (W×H×D)
- **Max Force:** 8N continuous, 15N peak
- **Resolution:** < 0.01mm positional
- **Update Rate:** Up to 4000 Hz (via HardwareAPI)
- **DOF:** 3 position, 3 force output
- **Interface:** USB-C (serial) or WebSocket (via Inverse Service)

---

## Getting Help

- **CHAI3D Forum:** https://www.chai3d.org/forum/
- **Haply Forum:** https://forum.haply.co/
- **Issue Tracker:** (your repository issues page)
- **Email:** pi.ko@nyu.edu

---

**Happy Haptic Coding!**
