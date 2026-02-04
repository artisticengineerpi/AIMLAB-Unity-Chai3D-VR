##############################################################################
# AIMLAB-Unity-Chai3D-VR - Top-Level CMake Configuration
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.3
# 
# Description:
#   Top-level CMake configuration for CHAI3D + Haply Inverse3 haptic 
#   application development. This project integrates CHAI3D haptic 
#   framework with Unity VR workflow for advanced haptic research.
#
# Prerequisites:
#   - Visual Studio 2022 with C++ workload
#   - CMake >= 3.10
#   - CHAI3D library built with ENABLE_HAPLY_DEVICES=ON
#   - Haply Hub and Inverse Service installed
#
# Build Instructions:
#   mkdir build
#   cd build
#   cmake .. -G "Visual Studio 17 2022" -A x64
#   cmake --build . --config Release
#
# Changelog:
#   v1.3 - 04 February 2026 - Added Windows OpenGL libraries (opengl32, glu32)
#   v1.2 - 04 February 2026 - Fixed FreeGLUT path (extras/freeglut not external/freeglut)
#   v1.1 - 04 February 2026 - Changed C++ standard to C++14 for CHAI3D Eigen compatibility
#   v1.0 - 04 February 2026 - Initial project structure setup
#
##############################################################################

cmake_minimum_required(VERSION 3.10)
project(AIMLAB-Haptics VERSION 1.0 LANGUAGES CXX)

# Use C++14 to maintain compatibility with CHAI3D's bundled Eigen library
# CHAI3D's Eigen uses std::binder1st/binder2nd which were removed in C++17
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─────────────────────────────────────────────────────────────────────────
# CHAI3D Configuration
# ─────────────────────────────────────────────────────────────────────────
# CHAI3D must be built first from external/chai3d directory.
# After building, CMake registers it so find_package works.
# 
# Build CHAI3D with:
#   cd external/chai3d
#   mkdir build && cd build
#   cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON
#   cmake --build . --config Release

find_package(CHAI3D REQUIRED)

# Include CHAI3D headers
include_directories(${CHAI3D_INCLUDE_DIRS})

# ─────────────────────────────────────────────────────────────────────────
# GLUT/OpenGL Configuration
# ─────────────────────────────────────────────────────────────────────────
if(WIN32)
    # On Windows, link against system OpenGL32 and GLU32
    set(OPENGL_LIBRARIES opengl32 glu32)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    set(OPENGL_LIBRARIES ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif()

# ─────────────────────────────────────────────────────────────────────────
# Application Target
# ─────────────────────────────────────────────────────────────────────────
add_executable(aimlab-haptics src/main.cpp)

# Link against CHAI3D and OpenGL libraries
target_link_libraries(aimlab-haptics ${CHAI3D_LIBRARIES} ${OPENGL_LIBRARIES})

# ─────────────────────────────────────────────────────────────────────────
# Output Directory Configuration
# ─────────────────────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ─────────────────────────────────────────────────────────────────────────
# Status Messages
# ─────────────────────────────────────────────────────────────────────────
message(STATUS "==============================================")
message(STATUS "AIMLAB-Haptics Configuration")
message(STATUS "==============================================")
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CHAI3D Include Dirs: ${CHAI3D_INCLUDE_DIRS}")
message(STATUS "CHAI3D Libraries: ${CHAI3D_LIBRARIES}")
message(STATUS "==============================================")
