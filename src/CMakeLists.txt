##############################################################################
# AIMLAB-Unity-Chai3D-VR - Source Directory CMake Configuration
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.0
# 
# Description:
#   CMake configuration for application source files. This file defines
#   the main application executable and its dependencies.
#
# Changelog:
#   v1.0 - 04 February 2026 - Initial source configuration
#
##############################################################################

# Main application executable
add_executable(aimlab-haptics
    main.cpp
)

# Link against CHAI3D and GLUT libraries
target_link_libraries(aimlab-haptics
    ${CHAI3D_LIBRARIES}
    ${GLUT_LIBRARIES}
)

# Set output directory for executable
set_target_properties(aimlab-haptics PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)
