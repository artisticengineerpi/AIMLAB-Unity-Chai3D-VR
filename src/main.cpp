/****************************************************************************
 * AIMLAB Haptics - Starter Application
 * 
 * Author: Pi Ko (pi.ko@nyu.edu)
 * Date: 04 February 2026
 * Version: v1.1
 * 
 * Description:
 *   Starter CHAI3D application with Haply Inverse3 haptic device support.
 *   This application demonstrates basic haptic rendering with a simple
 *   3D scene containing an interactive sphere. The Inverse3 device provides
 *   force feedback when the cursor interacts with the sphere.
 * 
 * Features:
 *   - Auto-detection of Haply Inverse3 device
 *   - Basic 3D scene with camera, lighting, and haptic object
 *   - Real-time haptic rendering thread (1 kHz+)
 *   - GLUT-based graphics rendering
 *   - Keyboard controls for interaction
 * 
 * Controls:
 *   - ESC or 'q': Quit application
 *   - 'f': Toggle fullscreen mode
 *   - Mouse drag: Rotate camera view
 *   - Scroll: Zoom in/out
 * 
 * Prerequisites:
 *   - CHAI3D library built with ENABLE_HAPLY_DEVICES=ON
 *   - Haply Hub running with Inverse Service >= 3.1.0
 *   - Inverse3 device connected, powered, and calibrated
 * 
 * Build Instructions:
 *   mkdir build && cd build
 *   cmake .. -G "Visual Studio 17 2022" -A x64
 *   cmake --build . --config Release
 *   .\bin\Release\aimlab-haptics.exe
 * 
 * Changelog:
 *   v1.1 - 04 February 2026 - Fixed class name from cSphere to cShapeSphere
 *   v1.0 - 04 February 2026 - Initial implementation
 * 
 * Based on:
 *   CHAI3D framework (www.chai3d.org)
 *   Haply CHAI3D fork with Inverse3 support
 * 
 ****************************************************************************/

#include "chai3d.h"

// Platform-specific GLUT includes
#ifdef _WIN32
    #include <GL/freeglut.h>    // Windows (bundled with CHAI3D)
#elif defined(__APPLE__)
    #include <GLUT/glut.h>       // macOS
#else
    #include <GL/glut.h>         // Linux
#endif

using namespace chai3d;
using namespace std;

//===========================================================================
// GLOBAL VARIABLES
//===========================================================================

// CHAI3D World and Scene Objects
cWorld* world;                              // 3D world container
cCamera* camera;                            // Virtual camera
cDirectionalLight* light;                   // Directional light source
cShapeSphere* sphere;                       // Interactive haptic sphere

// Haptic Device and Tool
cHapticDeviceHandler* handler;              // Haptic device manager
cGenericHapticDevicePtr hapticDevice;       // Pointer to haptic device
cToolCursor* tool;                          // Haptic cursor tool

// Simulation State
bool simulationRunning = false;             // Haptic thread running flag
bool simulationFinished = false;            // Haptic thread finished flag
cThread* hapticThread;                      // Haptic rendering thread

// Window Dimensions
int windowW = 1024;                         // Window width (pixels)
int windowH = 768;                          // Window height (pixels)

//===========================================================================
// FUNCTION DECLARATIONS
//===========================================================================

void updateHaptics();                       // Haptic thread loop
void updateGraphics();                      // Graphics rendering callback
void resizeWindow(int w, int h);           // Window resize callback
void keySelect(unsigned char key, int x, int y);  // Keyboard input callback
void close();                               // Cleanup and exit

//===========================================================================
// HAPTIC THREAD FUNCTION
//===========================================================================

/**
 * @brief Main haptic rendering loop (runs at 1 kHz+)
 * 
 * This function runs in a separate high-priority thread to ensure smooth
 * haptic feedback. It updates device position, computes interaction forces,
 * and sends force commands to the Inverse3.
 * 
 * Performance Notes:
 *   - Runs at CTHREAD_PRIORITY_HAPTICS (highest priority)
 *   - Target frequency: >= 1000 Hz for stable haptics
 *   - Keep computation minimal (no I/O, memory allocation, etc.)
 */
void updateHaptics() {
    simulationRunning = true;
    simulationFinished = false;

    while (simulationRunning) {
        // Update global positions of all objects
        world->computeGlobalPositions(true);

        // Update tool position from device
        tool->updateFromDevice();

        // Compute interaction forces between tool and objects
        tool->computeInteractionForces();

        // Send computed forces to haptic device
        tool->applyToDevice();
    }

    simulationFinished = true;
}

//===========================================================================
// GRAPHICS CALLBACKS
//===========================================================================

/**
 * @brief GLUT display callback - renders the 3D scene
 */
void updateGraphics() {
    // Render scene from camera viewpoint
    camera->renderView(windowW, windowH);

    // Swap front and back buffers (double buffering)
    glutSwapBuffers();

    // Request redraw if simulation is running
    if (simulationRunning) {
        glutPostRedisplay();
    }
}

/**
 * @brief GLUT reshape callback - handles window resize events
 * @param w New window width
 * @param h New window height
 */
void resizeWindow(int w, int h) {
    windowW = w;
    windowH = h;
}

/**
 * @brief GLUT keyboard callback - handles key press events
 * @param key ASCII code of pressed key
 * @param x Mouse x-coordinate at time of press
 * @param y Mouse y-coordinate at time of press
 */
void keySelect(unsigned char key, int x, int y) {
    // ESC or 'q' - Quit application
    if (key == 27 || key == 'q') {
        close();
    }

    // 'f' - Toggle fullscreen mode
    if (key == 'f') {
        glutFullScreen();
    }
}

//===========================================================================
// CLEANUP FUNCTION
//===========================================================================

/**
 * @brief Cleanup and graceful shutdown
 * 
 * Stops the haptic thread, closes the device connection, and deallocates
 * all CHAI3D objects before exiting.
 */
void close() {
    // Stop haptic thread
    simulationRunning = false;
    
    // Wait for thread to complete
    while (!simulationFinished) {
        cSleepMs(100);
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

/**
 * @brief Application entry point
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Exit status code
 */
int main(int argc, char* argv[]) {
    //-----------------------------------------------------------------------
    // GLUT INITIALIZATION
    //-----------------------------------------------------------------------
    cout << "========================================" << endl;
    cout << "AIMLAB Haptics Starter Application" << endl;
    cout << "Author: Pi Ko (pi.ko@nyu.edu)" << endl;
    cout << "Date: 04 February 2026" << endl;
    cout << "========================================" << endl << endl;

    glutInit(&argc, argv);
    glutInitWindowSize(windowW, windowH);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("AIMLAB - Haptic Environment");

    // Register GLUT callbacks
    glutDisplayFunc(updateGraphics);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(keySelect);

    //-----------------------------------------------------------------------
    // WORLD SETUP
    //-----------------------------------------------------------------------
    cout << "Creating 3D world..." << endl;
    world = new cWorld();
    world->m_backgroundColor.setBlack();

    //-----------------------------------------------------------------------
    // CAMERA SETUP
    //-----------------------------------------------------------------------
    cout << "Setting up camera..." << endl;
    camera = new cCamera(world);
    world->addChild(camera);
    camera->set(cVector3d(0.5, 0.0, 0.3),   // Camera position
                cVector3d(0.0, 0.0, 0.0),    // Look-at point
                cVector3d(0.0, 0.0, 1.0));   // Up direction

    // Set camera perspective projection
    camera->setClippingPlanes(0.01, 10.0);
    camera->setFieldViewAngleDeg(45);

    //-----------------------------------------------------------------------
    // LIGHTING SETUP
    //-----------------------------------------------------------------------
    cout << "Configuring lighting..." << endl;
    light = new cDirectionalLight(world);
    world->addChild(light);
    light->setEnabled(true);
    light->setDir(-1.0, -1.0, -1.0);
    light->m_ambient.set(0.3f, 0.3f, 0.3f);
    light->m_diffuse.set(0.7f, 0.7f, 0.7f);
    light->m_specular.set(1.0f, 1.0f, 1.0f);

    //-----------------------------------------------------------------------
    // HAPTIC DEVICE SETUP
    //-----------------------------------------------------------------------
    cout << "Detecting haptic devices..." << endl;
    handler = new cHapticDeviceHandler();

    // Get first available haptic device
    handler->getDevice(hapticDevice, 0);

    // Check if device was found
    if (hapticDevice == nullptr) {
        cerr << "ERROR: No haptic device detected!" << endl;
        cerr << "Please ensure:" << endl;
        cerr << "  1. Inverse3 is connected via USB-C" << endl;
        cerr << "  2. 24V power supply is connected" << endl;
        cerr << "  3. Device is calibrated (white LED)" << endl;
        cerr << "  4. Haply Hub / Inverse Service is running" << endl;
        close();
        return 1;
    }

    // Get device information
    cHapticDeviceInfo info = hapticDevice->getSpecifications();
    cout << "Device found: " << info.m_modelName << endl;
    cout << "Manufacturer: " << info.m_manufacturerName << endl;

    // Open connection to device
    if (hapticDevice->open()) {
        cout << "Device connection opened successfully" << endl;
    } else {
        cerr << "ERROR: Failed to open device connection" << endl;
        close();
        return 1;
    }

    // Calibrate device (if not already calibrated)
    if (hapticDevice->calibrate()) {
        cout << "Device calibrated successfully" << endl;
    }

    //-----------------------------------------------------------------------
    // HAPTIC TOOL SETUP
    //-----------------------------------------------------------------------
    cout << "Creating haptic cursor..." << endl;
    tool = new cToolCursor(world);
    world->addChild(tool);
    tool->setHapticDevice(hapticDevice);
    tool->setRadius(0.005);              // Cursor sphere radius (5mm)
    tool->setWorkspaceRadius(0.15);      // Workspace scaling
    tool->enableDynamicObjects(true);    // Enable dynamic object interaction
    tool->start();                       // Start the tool

    //-----------------------------------------------------------------------
    // SCENE OBJECT SETUP - Interactive Sphere
    //-----------------------------------------------------------------------
    cout << "Creating scene objects..." << endl;
    sphere = new cShapeSphere(0.03);     // 30mm radius sphere
    world->addChild(sphere);
    sphere->setLocalPos(0.0, 0.0, 0.0);  // Center of workspace

    // Configure haptic material properties
    sphere->m_material->setStiffness(1000.0);        // Stiffness (N/m)
    sphere->m_material->setStaticFriction(0.3);      // Static friction coefficient
    sphere->m_material->setDynamicFriction(0.2);     // Dynamic friction coefficient
    sphere->m_material->setViscosity(0.1);           // Viscosity
    
    // Configure visual appearance
    sphere->m_material->setRedCrimson();
    sphere->m_material->setSpecularLevel(0.8);
    sphere->m_material->setShininess(100);
    
    // Enable haptic and graphic rendering
    sphere->setHapticEnabled(true);
    sphere->setShowEnabled(true);

    //-----------------------------------------------------------------------
    // START HAPTIC THREAD
    //-----------------------------------------------------------------------
    cout << "Starting haptic rendering thread..." << endl;
    hapticThread = new cThread();
    hapticThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    //-----------------------------------------------------------------------
    // START GRAPHICS MAIN LOOP
    //-----------------------------------------------------------------------
    cout << endl << "Application ready!" << endl;
    cout << "Controls:" << endl;
    cout << "  ESC / 'q' - Quit" << endl;
    cout << "  'f' - Fullscreen" << endl;
    cout << "  Mouse drag - Rotate view" << endl;
    cout << endl;

    // Enter GLUT event loop (blocks until exit)
    glutMainLoop();

    return 0;
}
