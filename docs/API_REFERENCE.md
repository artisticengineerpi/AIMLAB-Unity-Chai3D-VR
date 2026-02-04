/****************************************************************************
 * AIMLAB-Unity-Chai3D-VR - API Quick Reference
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.0
 * 
 * Description:
 *   Quick reference guide for commonly used CHAI3D API functions and
 *   Haply Inverse3 device operations. For complete API documentation,
 *   see https://www.chai3d.org/download/doc/html/
 * 
 * Changelog:
 *   v1.0 - 04 February 2026 - Initial API reference
 * 
 ****************************************************************************/

# CHAI3D + Haply Inverse3 - API Quick Reference

---

## Haptic Device Operations

### Device Detection and Connection

```cpp
// Create device handler
cHapticDeviceHandler* handler = new cHapticDeviceHandler();

// Get number of available devices
int numDevices = handler->getNumDevices();

// Get device by index
cGenericHapticDevicePtr device;
handler->getDevice(device, 0);  // 0 = first device

// Open connection
bool success = device->open();

// Calibrate device
bool calibrated = device->calibrate();

// Get device specifications
cHapticDeviceInfo info = device->getSpecifications();
cout << "Model: " << info.m_modelName << endl;
cout << "Max Force: " << info.m_maxLinearForce << " N" << endl;
```

### Reading Device State

```cpp
// Get end-effector position (meters)
cVector3d position;
device->getPosition(position);

// Get linear velocity (m/s)
cVector3d velocity;
device->getLinearVelocity(velocity);

// Get orientation (rotation matrix)
cMatrix3d rotation;
device->getRotation(rotation);

// Get user switch states (buttons)
bool button0, button1;
device->getUserSwitch(0, button0);
device->getUserSwitch(1, button1);
```

### Sending Forces

```cpp
// Set force vector (Newtons)
cVector3d force(0.0, 0.0, 1.0);  // 1N upward
device->setForce(force);

// Set torque (N·m) - if device supports it
cVector3d torque(0.0, 0.0, 0.1);
device->setTorque(torque);
```

### Device Information

```cpp
cHapticDeviceInfo info = device->getSpecifications();

// Workspace dimensions
double workspace = info.m_workspaceRadius;  // meters

// Force capabilities
double maxForce = info.m_maxLinearForce;         // N
double maxTorque = info.m_maxAngularTorque;      // N·m
double maxStiffness = info.m_maxLinearStiffness; // N/m
double maxDamping = info.m_maxLinearDamping;     // N·s/m

// Position sensing
double posResolution = info.m_sensedPositionResolution;  // m
double angleResolution = info.m_sensedAngleResolution;   // rad

// Device capabilities (booleans)
bool hasPosition = info.m_sensedPosition;
bool hasRotation = info.m_sensedRotation;
bool hasGripper = info.m_sensedGripper;
bool hasForce = info.m_actuatedGripper;
```

---

## Scene Graph and Objects

### World and Camera

```cpp
// Create world
cWorld* world = new cWorld();
world->m_backgroundColor.setBlack();

// Create camera
cCamera* camera = new cCamera(world);
world->addChild(camera);

// Set camera position and orientation
camera->set(
    cVector3d(0.5, 0.0, 0.3),    // eye position
    cVector3d(0.0, 0.0, 0.0),     // look-at point
    cVector3d(0.0, 0.0, 1.0)      // up vector
);

// Set projection parameters
camera->setClippingPlanes(0.01, 10.0);        // near, far
camera->setFieldViewAngleDeg(45.0);           // FOV in degrees

// Render the scene
camera->renderView(windowWidth, windowHeight);
```

### Lighting

```cpp
// Directional light
cDirectionalLight* light = new cDirectionalLight(world);
world->addChild(light);
light->setEnabled(true);
light->setDir(-1.0, -1.0, -1.0);  // Direction vector

// Light colors
light->m_ambient.set(0.3f, 0.3f, 0.3f);    // Ambient
light->m_diffuse.set(0.7f, 0.7f, 0.7f);    // Diffuse
light->m_specular.set(1.0f, 1.0f, 1.0f);   // Specular

// Point light
cPointLight* pointLight = new cPointLight(world);
pointLight->setPos(0.0, 0.0, 1.0);
pointLight->setEnabled(true);

// Spot light
cSpotLight* spotLight = new cSpotLight(world);
spotLight->setPos(0.0, 0.0, 1.0);
spotLight->setDir(0.0, 0.0, -1.0);
spotLight->setCutOffAngleDeg(30.0);  // Cone angle
```

### Basic Shapes

```cpp
// Sphere
cShapeSphere* sphere = new cShapeSphere(0.03);  // radius in meters
world->addChild(sphere);
sphere->setLocalPos(0.0, 0.0, 0.0);

// Box
cShapeBox* box = new cShapeBox(0.05, 0.05, 0.05);  // width, height, depth
world->addChild(box);

// Cylinder
cShapeCylinder* cylinder = new cShapeCylinder(0.02, 0.02, 0.1);  // r1, r2, height
world->addChild(cylinder);

// Torus
cShapeTorus* torus = new cShapeTorus(0.05, 0.01);  // inner radius, outer radius
world->addChild(torus);

// Line
cShapeLine* line = new cShapeLine(
    cVector3d(0.0, 0.0, 0.0),   // point A
    cVector3d(0.1, 0.1, 0.1)    // point B
);
world->addChild(line);
```

### Mesh Objects

```cpp
// Create mesh
cMesh* mesh = new cMesh();

// Load from file
bool success = mesh->loadFromFile("../resources/bunny.obj");

if (success) {
    world->addChild(mesh);
    mesh->setLocalPos(0.0, 0.0, 0.0);
    mesh->scale(0.1);  // Scale factor
    
    // Create collision detector for haptics
    mesh->createAABBCollisionDetector(0.005);  // tool radius
    
    // Compute normals for smooth shading
    mesh->computeAllNormals();
}
```

---

## Material Properties

### Visual Properties

```cpp
// Color (predefined)
material->setRed();
material->setGreen();
material->setBlue();
material->setWhite();
material->setBlack();
material->setGray();
material->setRedCrimson();
material->setOrangeCoral();

// Color (custom RGB)
material->setColor(cColorf(1.0f, 0.5f, 0.0f));  // Orange

// Transparency
material->setTransparencyLevel(0.5);  // 0.0 = opaque, 1.0 = transparent

// Shininess
material->setShininess(100);           // 0-128
material->setSpecularLevel(0.8);       // 0.0-1.0
```

### Haptic Properties

```cpp
// Stiffness (surface hardness)
material->setStiffness(1000.0);  // N/m (0-3000 for Inverse3)

// Friction
material->setStaticFriction(0.5);    // 0.0-1.0
material->setDynamicFriction(0.3);   // 0.0-1.0

// Viscosity (damping)
material->setViscosity(5.0);  // N·s/m

// Vibration
material->setVibrationFrequency(200.0);  // Hz
material->setVibrationAmplitude(0.5);    // N
material->setUseHapticShading(true);

// Magnetic effect
material->setMagnetMaxForce(5.0);       // N
material->setMagnetMaxDistance(0.05);   // m
material->setUseMagnet(true);

// Stick-slip effect
material->setStickSlipForceMax(2.0);    // N
material->setStickSlipStiffness(500.0); // N/m
material->setUseStickSlip(true);
```

---

## Haptic Tool (Cursor)

### Creating and Configuring Tool

```cpp
// Create tool cursor
cToolCursor* tool = new cToolCursor(world);
world->addChild(tool);

// Connect to haptic device
tool->setHapticDevice(device);

// Set tool parameters
tool->setRadius(0.005);              // Cursor radius (5mm)
tool->setWorkspaceRadius(0.15);      // Workspace scaling
tool->setWaitForSmallForce(true);    // Wait for zero force before starting
tool->setShowContactPoints(true);    // Visualize contact points

// Enable/disable features
tool->enableDynamicObjects(true);    // Allow moving objects
tool->setShowEnabled(true);          // Show cursor sphere

// Start the tool
tool->start();
```

### Tool Update Loop (Haptic Thread)

```cpp
void updateHaptics() {
    while (simulationRunning) {
        // Update global positions
        world->computeGlobalPositions(true);
        
        // Update tool from device
        tool->updateFromDevice();
        
        // Compute forces
        tool->computeInteractionForces();
        
        // Apply forces to device
        tool->applyToDevice();
    }
}
```

### Getting Tool State

```cpp
// Get device position
cVector3d toolPos = tool->getDeviceLocalPos();

// Get proxy position (surface contact point)
cVector3d proxyPos = tool->getProxyGlobalPos();

// Check if in contact with object
bool inContact = tool->isInContact(object);

// Get current force
cVector3d force = tool->getDeviceLocalForce();
```

---

## Transformations

### Position and Orientation

```cpp
// Set local position
object->setLocalPos(cVector3d(0.1, 0.2, 0.0));
object->setLocalPos(0.1, 0.2, 0.0);  // Alternative syntax

// Get positions
cVector3d localPos = object->getLocalPos();
cVector3d globalPos = object->getGlobalPos();

// Set rotation (Euler angles, degrees)
object->rotateAboutGlobalAxisDeg(cVector3d(0,0,1), 45.0);  // Rotate 45° around Z
object->rotateAboutLocalAxisDeg(cVector3d(1,0,0), 30.0);   // Rotate 30° around local X

// Set rotation (rotation matrix)
cMatrix3d rotation;
rotation.identity();
rotation.rotateAboutGlobalAxisDeg(cVector3d(0,0,1), 45.0);
object->setLocalRot(rotation);

// Scale
object->scale(2.0);                          // Uniform scale
object->scale(2.0, 1.0, 0.5);               // Non-uniform scale
```

### Transform Matrices

```cpp
// Get transform matrix (4x4)
cTransform transform = object->getLocalTransform();

// Set transform
cTransform newTransform;
newTransform.setLocalPos(cVector3d(1, 0, 0));
newTransform.setLocalRot(rotation);
object->setLocalTransform(newTransform);
```

---

## Textures and Materials

### Loading Textures

```cpp
// Create texture
cTexture2dPtr texture = cTexture2d::create();

// Load from file
bool success = texture->loadFromFile("../resources/texture.png");

// Apply to object
object->setTexture(texture);
object->setUseTexture(true);

// Texture mapping modes
texture->setWrapModeS(GL_REPEAT);        // or GL_CLAMP
texture->setWrapModeT(GL_REPEAT);

// Texture filtering
texture->setMagFunction(GL_LINEAR);      // Magnification
texture->setMinFunction(GL_LINEAR_MIPMAP_LINEAR);  // Minification
```

### Normal Mapping

```cpp
// Load normal map
cNormalMapPtr normalMap = cNormalMap::create();
normalMap->loadFromFile("../resources/normal.png");

object->m_normalMap = normalMap;
object->setUseNormalMap(true);
```

---

## Collision Detection

### Setting Up Collision Trees

```cpp
// For mesh objects - AABB tree (fastest)
mesh->createAABBCollisionDetector(toolRadius);

// Show collision tree (debugging)
mesh->setShowBoundaryBox(true);
mesh->setShowCollisionDetector(true);

// Update collision detector after modifying mesh
mesh->updateCollisionDetector();
```

### Collision Events

```cpp
// In haptic loop - detect collision
cCollisionRecorder recorder;
cCollisionSettings settings;

bool collision = tool->computeCollisionDetection(
    toolPos, toolVel, recorder, settings
);

if (collision) {
    // Get collision point
    cVector3d collisionPoint = recorder.m_nearestCollision.m_globalPos;
    
    // Get surface normal
    cVector3d normal = recorder.m_nearestCollision.m_globalNormal;
    
    // Get collided object
    cGenericObject* object = recorder.m_nearestCollision.m_object;
}
```

---

## Vectors and Math

### Vector Operations

```cpp
// Create vectors
cVector3d v1(1.0, 2.0, 3.0);
cVector3d v2(4.0, 5.0, 6.0);

// Access components
double x = v1.x();
double y = v1.y();
double z = v1.z();

// Or use operator()
double x = v1(0);
double y = v1(1);
double z = v1(2);

// Vector operations
cVector3d sum = v1 + v2;           // Addition
cVector3d diff = v1 - v2;          // Subtraction
cVector3d scaled = 2.0 * v1;       // Scalar multiplication
double dot = v1.dot(v2);           // Dot product
cVector3d cross = cCross(v1, v2);  // Cross product

// Vector properties
double length = v1.length();       // Magnitude
double lengthSq = v1.lengthsq();   // Squared length (faster)

// Normalize vector
v1.normalize();                    // Modifies v1
cVector3d normalized = cNormalize(v1);  // Returns new vector

// Distance between two points
double dist = cDistance(v1, v2);
```

### Matrix Operations

```cpp
// Create 3x3 rotation matrix
cMatrix3d R;
R.identity();  // Identity matrix

// Rotate matrix
R.rotateAboutGlobalAxisDeg(cVector3d(0,0,1), 45.0);

// Matrix-vector multiplication
cVector3d rotated = R * v1;

// Matrix multiplication
cMatrix3d R2;
cMatrix3d result = R * R2;

// Transpose
cMatrix3d RT = cTranspose(R);

// Get Euler angles (radians)
double roll, pitch, yaw;
R.toAxisAngle(axis, angle);
```

---

## Utilities

### Timing and Threading

```cpp
// High-resolution clock
cPrecisionClock clock;
clock.start();
double elapsedSec = clock.getCurrentTimeSeconds();
clock.stop();

// Frequency counter (for haptic rate monitoring)
cFrequencyCounter freqCounter;
freqCounter.signal(1);  // Call each iteration
double frequency = freqCounter.getFrequency();  // Hz

// Threading
cThread* thread = new cThread();
thread->start(myFunction, CTHREAD_PRIORITY_HAPTICS);

// Sleep
cSleepMs(100);  // Sleep for 100 milliseconds
```

### File I/O

```cpp
// Load 3D model
cMultiMesh* model = new cMultiMesh();
bool success = model->loadFromFile("model.obj");

// Load image
cImagePtr image = cImage::create();
success = image->loadFromFile("image.png");

// File existence check
bool exists = cFileExists("path/to/file.txt");
```

### Math Utilities

```cpp
// Clamping
double clamped = cClamp(value, min, max);

// Linear interpolation
double lerp = cLerp(t, value0, value1);  // t in [0,1]

// Angle conversion
double radians = cDegToRad(45.0);
double degrees = cRadToDeg(1.57);

// Random numbers
double random01 = cRandom();              // [0, 1]
double randomRange = cRandomRange(-1, 1); // [-1, 1]

// Sign function
double sign = cSign(value);  // -1, 0, or 1
```

---

## Audio

```cpp
// Create audio source
cAudioSource* audioSource = new cAudioSource();

// Load sound file
bool success = audioSource->loadFromFile("sound.wav");

// Play sound
audioSource->play();

// Set properties
audioSource->setLoop(true);          // Loop playback
audioSource->setGain(0.8);           // Volume (0.0-1.0)
audioSource->setPitch(1.0);          // Pitch multiplier

// 3D spatial audio
audioSource->setSourcePos(cVector3d(1, 0, 0));
audioSource->setSourceVelocity(cVector3d(0, 0, 0));

// Audio device (listener)
cAudioDevice* audioDevice = new cAudioDevice();
audioDevice->setListenerPos(cVector3d(0, 0, 0));
audioDevice->setListenerRot(cIdentity3d());
```

---

## Advanced Features

### Multi-Mesh (Complex Models)

```cpp
// Load multi-part model (e.g., robot with separate parts)
cMultiMesh* robot = new cMultiMesh();
robot->loadFromFile("robot.obj");

// Access individual meshes
int numMeshes = robot->getNumMeshes();
for (int i = 0; i < numMeshes; i++) {
    cMesh* part = robot->getMesh(i);
    part->m_material->setRed();
}
```

### Labels (2D Text)

```cpp
// Create font
cFontPtr font = NEW_CFONTCALIBRI20();

// Create label
cLabel* label = new cLabel(font);
world->addChild(label);
label->setText("Hello Haptics!");
label->setLocalPos(0.1, 0.1, 0.0);
label->m_fontColor.setBlack();
```

### Voxel (Volumetric) Data

```cpp
// Create voxel object
cVoxelObject* voxels = new cVoxelObject();

// Load from file (raw, nrrd, etc.)
voxels->loadFromFile("volume.nrrd");

// Configure rendering
voxels->setQuality(0.5);               // 0.0-1.0
voxels->setOpacityThreshold(0.1);      // Transparency cutoff
voxels->setIsosurfaceValue(0.5);       // Isosurface level

// Set colormap
voxels->m_colorMap->setTransparencyLevel(0.5);
```

---

## Performance Best Practices

### Haptic Thread Optimization

```cpp
// ✅ DO THIS - Pre-allocate outside loop
cVector3d force;
double stiffness = 1000.0;

void updateHaptics() {
    while (running) {
        // Minimal computation
        device->getPosition(position);
        force = -stiffness * position;
        device->setForce(force);
    }
}

// ❌ DON'T DO THIS - Allocation in loop
void updateHaptics() {
    while (running) {
        cVector3d position;  // ❌ Allocates every iteration
        device->getPosition(position);
        // ...
    }
}
```

### Graphics Optimization

```cpp
// Use display lists for static geometry
object->setUseDisplayList(true);

// Level of detail (LOD)
object->setDisplayLevel(2);  // 0=high detail, higher=lower detail

// Frustum culling
camera->setUseFrustumCulling(true);

// Backface culling
object->setUseCulling(true);
```

---

## Haply Inverse3 Specifications

```cpp
// Workspace (meters)
// X: ±0.08 m (160mm total)
// Y: ±0.05 m (100mm total)  
// Z: 0.0 to 0.13 m (130mm total)

// Force
// Continuous: 8N
// Peak: 15N

// Stiffness
// Recommended max: 3000 N/m

// Update Rate
// Via Inverse Service: ~1000 Hz
// Via HardwareAPI: up to 4000 Hz

// Position Resolution
// < 0.01 mm
```

---

## Common Patterns

### Spring Force to Point

```cpp
cVector3d anchor(0.0, 0.0, 0.1);
cVector3d position;
device->getPosition(position);

double k = 500.0;  // Spring constant (N/m)
cVector3d force = k * (anchor - position);

device->setForce(force);
```

### Gravity Well

```cpp
cVector3d wellCenter(0.0, 0.0, 0.0);
cVector3d position;
device->getPosition(position);

cVector3d toCenter = wellCenter - position;
double distance = toCenter.length();

if (distance > 0.001) {
    toCenter.normalize();
    double G = 10.0;  // Gravitational constant
    double forceMag = G / (distance * distance);
    cVector3d force = forceMag * toCenter;
    
    // Clamp to max force
    if (force.length() > 8.0) {
        force.normalize();
        force *= 8.0;
    }
    
    device->setForce(force);
}
```

### Haptic Boundary

```cpp
// Constrain to sphere boundary
cVector3d center(0.0, 0.0, 0.05);
double radius = 0.1;

cVector3d position;
device->getPosition(position);

cVector3d fromCenter = position - center;
double dist = fromCenter.length();

if (dist > radius) {
    // Outside boundary - push back
    fromCenter.normalize();
    cVector3d surfacePoint = center + radius * fromCenter;
    
    double k = 2000.0;  // Stiffness
    cVector3d force = k * (surfacePoint - position);
    
    device->setForce(force);
}
```

---

## Debugging and Diagnostics

### Console Output

```cpp
// Print vector
cVector3d v(1, 2, 3);
v.print();  // Outputs: (1.00, 2.00, 3.00)

// Print matrix
cMatrix3d R;
R.print();

// Manual printf
printf("Position: [%.3f, %.3f, %.3f]\n", pos.x(), pos.y(), pos.z());
```

### Visual Debugging

```cpp
// Show coordinate frames
object->setShowFrame(true);
object->setFrameSize(0.05);  // Frame size in meters

// Show bounding box
object->setShowBoundaryBox(true);

// Show collision detector
object->setShowCollisionDetector(true);

// Show normals
mesh->setShowNormals(true);
mesh->setNormalsLength(0.01);
```

---

## Complete Example - Haptic Button

```cpp
// Create button object
cShapeBox* button = new cShapeBox(0.03, 0.03, 0.01);
world->addChild(button);
button->setLocalPos(0.0, 0.05, 0.0);
button->m_material->setBlue();
button->m_material->setStiffness(500.0);
button->setHapticEnabled(true);

bool buttonPressed = false;
double pressThreshold = 0.005;  // 5mm press depth

// In haptic loop
cVector3d buttonPos = button->getLocalPos();
cVector3d toolPos = tool->getDeviceLocalPos();

if (cDistance(buttonPos, toolPos) < pressThreshold) {
    if (!buttonPressed) {
        buttonPressed = true;
        cout << "Button pressed!" << endl;
        button->m_material->setRed();  // Visual feedback
        
        // Add haptic click feedback
        cVector3d clickForce(0, 0, -2.0);  // 2N downward pulse
        device->setForce(clickForce);
    }
} else {
    if (buttonPressed) {
        buttonPressed = false;
        cout << "Button released!" << endl;
        button->m_material->setBlue();
    }
}
```

---

## Resources

- **Full CHAI3D API:** https://www.chai3d.org/download/doc/html/
- **Haply Docs:** https://docs.haply.co/
- **Code Examples:** `external/chai3d/examples/`
- **Development Guide:** `docs/DEVELOPMENT.md`

---

**Author:** Pi Ko (pi.ko@nyu.edu)  
**Last Updated:** 04 February 2026
