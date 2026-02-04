/****************************************************************************
 * AIMLAB Haptics - Starter Application
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v2.0
 * 
 * Description:
 *   Starter CHAI3D application with Haply haptic device support.
 *   This application demonstrates basic haptic rendering with a simple
 *   3D scene containing an interactive sphere.
 * 
 *   IMPORTANT - DEVICE COMPATIBILITY:
 *   ===================================
 *   This build uses the GitHub CHAI3D fork (HaplyHaptics/chai3d) which
 *   ONLY supports the old Haply Pantograph (2-DOF) device via direct
 *   serial protocol (Haply-API-cpp).
 * 
 *   For INVERSE3 support, you need:
 *     - Official Haply CHAI3D v3.3.5 from https://develop.haply.co/releases/chai3d
 *     - OR GitLab source: https://gitlab.com/Haply/public/chai3d-demos
 *     - Uses Inverse SDK (WebSocket, not direct serial)
 *     - Requires Haply Inverse Service >= 3.1.0 running
 * 
 *   Device Support Matrix:
 *   ----------------------
 *   | Device          | This Build | Official CHAI3D | Protocol           |
 *   |-----------------|------------|-----------------|-------------------|
 *   | Pantograph 2DOF | ✅ YES     | ✅ YES          | Serial (Haply-API)|
 *   | Inverse3 3DOF   | ❌ NO*     | ✅ YES          | Inverse SDK (WS)  |
 * 
 *   * Inverse3 will be detected but position will always read 0,0,0 due to
 *     protocol mismatch. See docs/INVERSE3_PROTOCOL_NOTES.md for details.
 * 
 *   The application gracefully handles missing haptic devices — it will
 *   still render the 3D scene for visual preview even if no device is
 *   detected or cannot be opened.
 * 
 * Features:
 *   - Auto-detection of Haply devices
 *   - Graceful fallback when no device is connected
 *   - Basic 3D scene with camera, lighting, and haptic object
 *   - Real-time haptic rendering thread (1 kHz+)
 *   - GLUT-based graphics rendering
 *   - Keyboard controls for interaction
 * 
 * Controls:
 *   - ESC or 'q': Quit application
 *   - 'f': Toggle fullscreen mode
 * 
 * Prerequisites (Current Build):
 *   - CHAI3D library from GitHub fork (included as submodule)
 *   - For Pantograph: Device connected and calibrated
 *   - For Inverse3: Use official CHAI3D demos instead (see docs/)
 * 
 * Build Instructions:
 *   .\setup-chai3d.ps1    # One-time setup
 *   .\rebuild.ps1         # Full rebuild
 *   .\run.ps1             # Run with auto-cleanup
 * 
 * Testing with Inverse3:
 *   Use official pre-built demos instead:
 *   .\run-official-demos.ps1
 * 
 * Changelog:
 *   v2.0 - 04 February 2026 - Major documentation update: clarified device compatibility,
 *                              protocol limitations, added Inverse3 guidance, reorganized docs
 *   v1.6 - 04 February 2026 - Added position debugging and setShowEnabled(true) for cursor
 *   v1.5 - 04 February 2026 - Larger visible cursor (15mm), wider workspace (1.0m) for Pantograph
 *   v1.4 - 04 February 2026 - Graceful no-device handling; runs graphics without haptics
 *   v1.3 - 04 February 2026 - Changed to GL/glut.h, static runtime linkage
 *   v1.2 - 04 February 2026 - Fixed specular property
 *   v1.1 - 04 February 2026 - Fixed cSphere -> cShapeSphere
 *   v1.0 - 04 February 2026 - Initial implementation
 * 
 * Based on:
 *   CHAI3D framework (www.chai3d.org)
 *   GitHub fork: HaplyHaptics/chai3d (Pantograph protocol only)
 * 
 * See Also:
 *   docs/INVERSE3_PROTOCOL_NOTES.md  - Detailed protocol explanation
 *   docs/USING_OFFICIAL_CHAI3D.md    - How to use official Inverse3 demos
 *   docs/REAL_WORLD_SETUP_GUIDE.md   - Complete setup troubleshooting
 * 
 ****************************************************************************/

#include "chai3d.h"

// Platform-specific GLUT includes
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

using namespace chai3d;
using namespace std;

//===========================================================================
// GLOBAL VARIABLES
//===========================================================================

// CHAI3D World and Scene Objects
cWorld* world;
cCamera* camera;
cDirectionalLight* light;
cShapeSphere* sphere;

// Haptic Device and Tool
cHapticDeviceHandler* handler;
cGenericHapticDevicePtr hapticDevice;
cToolCursor* tool;

// Simulation State
bool simulationRunning  = false;
bool simulationFinished = true;     // Start true so close() doesn't hang if haptics never started
cThread* hapticThread;

// Device State
bool hapticDeviceConnected = false;  // Whether a haptic device was successfully initialized

// Window Dimensions
int windowW = 1024;
int windowH = 768;

//===========================================================================
// FUNCTION DECLARATIONS
//===========================================================================

void updateHaptics();
void updateGraphics();
void resizeWindow(int w, int h);
void keySelect(unsigned char key, int x, int y);
void close();

//===========================================================================
// HAPTIC THREAD FUNCTION
//===========================================================================

void updateHaptics() {
    simulationRunning  = true;
    simulationFinished = false;

    while (simulationRunning) {
        world->computeGlobalPositions(true);
        tool->updateFromDevice();
        tool->computeInteractionForces();
        tool->applyToDevice();
    }

    simulationFinished = true;
}

//===========================================================================
// GRAPHICS CALLBACKS
//===========================================================================

void updateGraphics() {
    camera->renderView(windowW, windowH);

    // Debug: print tool position to console every ~60 frames
    if (hapticDeviceConnected) {
        static int frameCount = 0;
        static bool warnedZeroPosition = false;
        if (++frameCount % 60 == 0) {
            cVector3d pos = tool->getDeviceGlobalPos();
            cout << "[debug] Tool pos: "
                 << pos.x() << ", "
                 << pos.y() << ", "
                 << pos.z() << endl;

            // Check for persistent zero position (Inverse3 protocol mismatch symptom)
            if (!warnedZeroPosition && frameCount > 300) {  // After 5 seconds @ 60fps
                if (pos.length() < 0.001) {  // Essentially zero
                    cout << endl;
                    cout << "  ⚠️  Position is stuck at (0,0,0)" << endl;
                    cout << "  If you have Inverse3, this is a protocol mismatch." << endl;
                    cout << "  Use: .\\run-official-demos.ps1" << endl;
                    cout << "  See: docs/INVERSE3_PROTOCOL_NOTES.md" << endl;
                    cout << endl;
                    warnedZeroPosition = true;
                }
            }
        }
    }

    glutSwapBuffers();

    // Keep redrawing
    glutPostRedisplay();
}

void resizeWindow(int w, int h) {
    windowW = w;
    windowH = h;
}

void keySelect(unsigned char key, int x, int y) {
    if (key == 27 || key == 'q') {
        close();
    }

    if (key == 'f') {
        glutFullScreen();
    }
}

//===========================================================================
// CLEANUP FUNCTION
//===========================================================================

void close() {
    // Stop haptic thread
    simulationRunning = false;

    // Wait for haptic thread to finish (only if it was started)
    if (hapticDeviceConnected) {
        while (!simulationFinished) {
            cSleepMs(100);
        }
    }

    // Close haptic device connection
    if (hapticDevice != nullptr) {
        hapticDevice->close();
    }

    // Delete allocated objects
    delete hapticThread;
    delete world;
    delete handler;

    exit(0);
}

//===========================================================================
// MAIN FUNCTION
//===========================================================================

int main(int argc, char* argv[]) {
    //-----------------------------------------------------------------------
    // BANNER
    //-----------------------------------------------------------------------
    cout << endl;
    cout << "========================================" << endl;
    cout << "  AIMLAB Haptics Starter Application"    << endl;
    cout << "  Author: Pi Ko (pi.ko@nyu.edu)"        << endl;
    cout << "  Date:   04 February 2026"              << endl;
    cout << "  Version: v2.0"                         << endl;
    cout << "========================================" << endl;
    cout << "  Device Support:"                        << endl;
    cout << "    ✅ Pantograph (2-DOF)"                << endl;
    cout << "    ❌ Inverse3 (use official demos)"     << endl;
    cout << "========================================" << endl;
    cout << endl;

    //-----------------------------------------------------------------------
    // GLUT INITIALIZATION
    //-----------------------------------------------------------------------
    glutInit(&argc, argv);
    glutInitWindowSize(windowW, windowH);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("AIMLAB - Haptic Environment");
    glutDisplayFunc(updateGraphics);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(keySelect);

    //-----------------------------------------------------------------------
    // WORLD
    //-----------------------------------------------------------------------
    cout << "[init] Creating 3D world..." << endl;
    world = new cWorld();
    world->m_backgroundColor.setBlack();

    //-----------------------------------------------------------------------
    // CAMERA
    //-----------------------------------------------------------------------
    cout << "[init] Setting up camera..." << endl;
    camera = new cCamera(world);
    world->addChild(camera);
    camera->set(cVector3d(0.5, 0.0, 0.3),   // eye
                cVector3d(0.0, 0.0, 0.0),    // lookat
                cVector3d(0.0, 0.0, 1.0));   // up
    camera->setClippingPlanes(0.01, 10.0);
    camera->setFieldViewAngleDeg(45);

    //-----------------------------------------------------------------------
    // LIGHTING
    //-----------------------------------------------------------------------
    cout << "[init] Configuring lighting..." << endl;
    light = new cDirectionalLight(world);
    world->addChild(light);
    light->setEnabled(true);
    light->setDir(-1.0, -1.0, -1.0);
    light->m_ambient.set(0.3f, 0.3f, 0.3f);
    light->m_diffuse.set(0.7f, 0.7f, 0.7f);
    light->m_specular.set(1.0f, 1.0f, 1.0f);

    //-----------------------------------------------------------------------
    // SCENE OBJECTS
    //-----------------------------------------------------------------------
    cout << "[init] Creating scene objects..." << endl;
    sphere = new cShapeSphere(0.03);     // 30 mm radius
    world->addChild(sphere);
    sphere->setLocalPos(0.0, 0.0, 0.0);

    // Haptic material
    sphere->m_material->setStiffness(1000.0);
    sphere->m_material->setStaticFriction(0.3);
    sphere->m_material->setDynamicFriction(0.2);
    sphere->m_material->setViscosity(0.1);

    // Visual appearance
    sphere->m_material->setRedCrimson();
    sphere->m_material->m_specular.set(0.8f, 0.8f, 0.8f);
    sphere->m_material->setShininess(100);

    sphere->setHapticEnabled(true);
    sphere->setShowEnabled(true);

    //-----------------------------------------------------------------------
    // HAPTIC DEVICE (with graceful fallback)
    //-----------------------------------------------------------------------
    // NOTE: This build uses the GitHub CHAI3D fork which supports:
    //   ✅ Haply Pantograph (2-DOF) - works correctly
    //   ❌ Haply Inverse3 (3-DOF) - protocol mismatch, position always 0,0,0
    //
    // For proper Inverse3 support, see:
    //   - docs/INVERSE3_PROTOCOL_NOTES.md (detailed explanation)
    //   - docs/USING_OFFICIAL_CHAI3D.md (how to use official demos)
    //   - Run: .\run-official-demos.ps1 (34 pre-built Inverse3-compatible demos)
    //
    // What needs to change for Inverse3:
    //   1. Use official Haply CHAI3D library (not GitHub fork)
    //   2. Official library uses Inverse SDK (WebSocket protocol)
    //   3. Connects to Inverse Service at ws://localhost:10001
    //   4. Haply Hub MUST be running (opposite of current requirement)
    //   5. Device class: Haply::HardwareAPI::Devices::Inverse3
    //   6. Methods: GetEndEffectorPosition(), SendEndEffectorForce()
    //
    // Example Inverse3 initialization (with official CHAI3D):
    //   handler->getDevice(hapticDevice, 0);  // Same API surface
    //   hapticDevice->open();                 // Internally uses Inverse SDK
    //   // Position and force work correctly through WebSocket
    //
    // For now, this code attempts generic device detection and will
    // gracefully fall back to visual-only mode if device fails.
    //-----------------------------------------------------------------------

    cout << "[init] Detecting haptic devices..." << endl;
    handler = new cHapticDeviceHandler();
    handler->getDevice(hapticDevice, 0);

    if (hapticDevice == nullptr) {
        // ------------------------------------------------------------------
        // NO DEVICE AT ALL
        // ------------------------------------------------------------------
        cout << endl;
        cout << "  =============================================" << endl;
        cout << "  WARNING: No haptic device detected." << endl;
        cout << "  =============================================" << endl;
        cout << "  The application will run in VISUAL-ONLY mode." << endl;
        cout << "  You can view the 3D scene but haptic feedback" << endl;
        cout << "  is disabled." << endl;
        cout << endl;
        cout << "  Device Support:" << endl;
        cout << "    ✅ Haply Pantograph (2-DOF) - fully supported" << endl;
        cout << "    ❌ Haply Inverse3 (3-DOF) - use official demos" << endl;
        cout << endl;
        cout << "  For Pantograph:" << endl;
        cout << "    1. Connect device via USB" << endl;
        cout << "    2. Close Haply Hub (port conflict)" << endl;
        cout << "    3. Run: .\\run.ps1" << endl;
        cout << endl;
        cout << "  For Inverse3:" << endl;
        cout << "    1. Download: https://develop.haply.co/releases/chai3d" << endl;
        cout << "    2. Run: .\\run-official-demos.ps1" << endl;
        cout << "    3. See: docs/USING_OFFICIAL_CHAI3D.md" << endl;
        cout << "  =============================================" << endl;
        cout << endl;

        hapticDeviceConnected = false;

    } else {
        // ------------------------------------------------------------------
        // DEVICE FOUND — attempt to open it
        // ------------------------------------------------------------------
        cHapticDeviceInfo info = hapticDevice->getSpecifications();
        cout << "[init] Device found: " << info.m_modelName << endl;
        cout << "[init] Manufacturer: " << info.m_manufacturerName << endl;

        // Check if detected device is Inverse3
        if (info.m_modelName.find("Inverse3") != std::string::npos) {
            cout << endl;
            cout << "  =============================================" << endl;
            cout << "  IMPORTANT: Inverse3 Detected!" << endl;
            cout << "  =============================================" << endl;
            cout << "  This build uses the GitHub CHAI3D fork which" << endl;
            cout << "  does NOT support Inverse3 protocol." << endl;
            cout << endl;
            cout << "  Symptoms you may experience:" << endl;
            cout << "    - Position always reads (0, 0, 0)" << endl;
            cout << "    - Device appears connected but doesn't track" << endl;
            cout << "    - Force feedback may not work" << endl;
            cout << endl;
            cout << "  Solution - Use Official Demos:" << endl;
            cout << "    .\\run-official-demos.ps1" << endl;
            cout << endl;
            cout << "  For details:" << endl;
            cout << "    docs/INVERSE3_PROTOCOL_NOTES.md" << endl;
            cout << "    docs/USING_OFFICIAL_CHAI3D.md" << endl;
            cout << "  =============================================" << endl;
            cout << endl;
            cout << "  Attempting to continue anyway..." << endl;
            cout << endl;
        }

        // Check for "no device" placeholder that CHAI3D returns on failure
        if (info.m_modelName == "no device") {
            cout << endl;
            cout << "  =============================================" << endl;
            cout << "  WARNING: Serial port error (ACCESS_DENIED)." << endl;
            cout << "  =============================================" << endl;
            cout << "  CHAI3D detected a serial port but could not"  << endl;
            cout << "  open it. This usually means another process"  << endl;
            cout << "  (e.g., Haply Hub) is holding the port."       << endl;
            cout << endl;
            cout << "  Fix:" << endl;
            cout << "    - Close Haply Hub" << endl;
            cout << "    - Run: .\\run.ps1 (auto-kills background services)" << endl;
            cout << "  =============================================" << endl;
            cout << endl;
            cout << "  Continuing in VISUAL-ONLY mode..." << endl;
            cout << endl;

            hapticDeviceConnected = false;

        } else if (!hapticDevice->open()) {
            cout << endl;
            cout << "  WARNING: Could not open device connection." << endl;
            cout << "  Continuing in VISUAL-ONLY mode..." << endl;
            cout << endl;

            hapticDeviceConnected = false;

        } else {
            // Device opened successfully
            cout << "[init] Device connection opened successfully." << endl;

            hapticDevice->calibrate();
            cout << "[init] Device calibrated." << endl;

            hapticDeviceConnected = true;

            // If Pantograph detected, confirm it's working
            if (info.m_modelName.find("Pantograph") != std::string::npos) {
                cout << "[init] Pantograph protocol active." << endl;
                cout << "[init] Device should work correctly!" << endl;
            }
        }
    }

    //-----------------------------------------------------------------------
    // HAPTIC TOOL (only if device is connected)
    //-----------------------------------------------------------------------
    if (hapticDeviceConnected) {
        cout << "[init] Creating haptic cursor..." << endl;
        tool = new cToolCursor(world);
        world->addChild(tool);
        tool->setHapticDevice(hapticDevice);
        tool->setRadius(0.015);              // Larger cursor (15mm) so it's visible
        tool->setWorkspaceRadius(1.0);       // Wider workspace mapping for Pantograph
        tool->enableDynamicObjects(true);
        tool->start();

        // Make cursor visible with bright colors
        tool->setShowEnabled(true);          // Ensure cursor rendering is enabled
        tool->m_hapticPoint->m_sphereProxy->m_material->setWhite();
        tool->m_hapticPoint->m_sphereGoal->m_material->setYellowGold();

        cout << "[init] Starting haptic rendering thread..." << endl;
        hapticThread = new cThread();
        hapticThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);
    }

    //-----------------------------------------------------------------------
    // READY
    //-----------------------------------------------------------------------
    cout << endl;
    cout << "========================================" << endl;
    if (hapticDeviceConnected) {
        cout << "  Application ready (haptics ENABLED)"  << endl;
    } else {
        cout << "  Application ready (VISUAL-ONLY mode)" << endl;
    }
    cout << "========================================" << endl;
    cout << "  Controls:" << endl;
    cout << "    ESC / 'q' - Quit" << endl;
    cout << "    'f'       - Fullscreen" << endl;
    cout << "========================================" << endl;
    cout << endl;

    // Show helpful next steps if no haptics
    if (!hapticDeviceConnected) {
        cout << "  💡 Next Steps:" << endl;
        cout << "     - For Pantograph: Close Haply Hub, run .\\run.ps1" << endl;
        cout << "     - For Inverse3: Run .\\run-official-demos.ps1" << endl;
        cout << "     - See docs/ folder for troubleshooting guides" << endl;
        cout << endl;
    }

    //-----------------------------------------------------------------------
    // MAIN LOOP
    //-----------------------------------------------------------------------
    glutMainLoop();

    return 0;
}
