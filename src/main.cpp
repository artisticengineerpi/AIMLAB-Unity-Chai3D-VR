/****************************************************************************
 * AIMLAB Haptics - Starter Application
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.6
 * 
 * Description:
 *   Starter CHAI3D application with Haply Inverse3 haptic device support.
 *   This application demonstrates basic haptic rendering with a simple
 *   3D scene containing an interactive sphere. The Inverse3 device provides
 *   force feedback when the cursor interacts with the sphere.
 * 
 *   The application gracefully handles missing haptic devices — it will
 *   still render the 3D scene for visual preview even if no device is
 *   detected or if the Haply Hub is not running.
 * 
 * Features:
 *   - Auto-detection of Haply Inverse3 device
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
 * Prerequisites:
 *   - CHAI3D library built with ENABLE_HAPLY_DEVICES=ON
 *   - Haply Hub running with Inverse Service >= 3.1.0 (for haptics)
 *   - Inverse3 device connected, powered, and calibrated (for haptics)
 * 
 * Build Instructions:
 *   mkdir build && cd build
 *   cmake .. -G "Visual Studio 17 2022" -A x64
 *   cmake --build . --config Release
 *   .\bin\Release\aimlab-haptics.exe
 * 
 * Changelog:
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
 *   Haply CHAI3D fork with Inverse3 support
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
        if (++frameCount % 60 == 0) {
            cVector3d pos = tool->getDeviceGlobalPos();
            cout << "[debug] Tool pos: "
                 << pos.x() << ", "
                 << pos.y() << ", "
                 << pos.z() << endl;
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
        cout << "  To enable haptics, make sure:" << endl;
        cout << "    1. Haply Hub is installed and running" << endl;
        cout << "    2. Inverse Service >= 3.1.0 is active" << endl;
        cout << "    3. Inverse3 is connected via USB-C" << endl;
        cout << "    4. 24V power supply is plugged in" << endl;
        cout << "    5. Device LED is solid white (calibrated)" << endl;
        cout << "    6. Close Haply Hub before launching this app" << endl;
        cout << "       (Hub may hold exclusive serial port access)" << endl;
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

        // Check for "no device" placeholder that CHAI3D returns on failure
        if (info.m_modelName == "no device") {
            cout << endl;
            cout << "  =============================================" << endl;
            cout << "  WARNING: Serial port error (ACCESS_DENIED)." << endl;
            cout << "  =============================================" << endl;
            cout << "  CHAI3D detected a serial port but could not"  << endl;
            cout << "  open it. This usually means Haply Hub is"     << endl;
            cout << "  holding the port exclusively."                 << endl;
            cout << endl;
            cout << "  Fix: Close Haply Hub, then re-launch this app." << endl;
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

    //-----------------------------------------------------------------------
    // MAIN LOOP
    //-----------------------------------------------------------------------
    glutMainLoop();

    return 0;
}
