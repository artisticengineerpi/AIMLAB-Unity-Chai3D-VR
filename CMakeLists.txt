##############################################################################
# AIMLAB-Unity-Chai3D-VR - Top-Level CMake Configuration
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.6
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
#   v1.6 - 04 February 2026 - Force /MT flags directly (CMAKE_MSVC_RUNTIME_LIBRARY didn't work)
#   v1.5 - 04 February 2026 - Fixed runtime library mismatch (use /MT to match CHAI3D)
#   v1.4 - 04 February 2026 - Build FreeGLUT from source using add_subdirectory()
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

# Match CHAI3D's runtime library setting (static /MT)
# CHAI3D uses MT_StaticRelease, so we must use /MT to avoid linker errors
if(MSVC)
    # Force static runtime library for all configurations
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
    
    # Remove any /MD or /MDd flags that might be set by default
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif()

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
# FreeGLUT Configuration (Build from CHAI3D bundled source)
# ─────────────────────────────────────────────────────────────────────────
if(WIN32)
    # Build FreeGLUT as a static library from CHAI3D's bundled source
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/chai3d/extras/freeglut 
                     ${CMAKE_CURRENT_BINARY_DIR}/freeglut)
    # FreeGLUT exports FREEGLUT_INCLUDE_DIRS and FREEGLUT_LIBRARIES to parent scope
    include_directories(${FREEGLUT_INCLUDE_DIRS})
else()
    find_package(GLUT REQUIRED)
    set(FREEGLUT_LIBRARIES ${GLUT_LIBRARIES})
endif()

# ─────────────────────────────────────────────────────────────────────────
# Application Target
# ─────────────────────────────────────────────────────────────────────────
add_executable(aimlab-haptics src/main.cpp)

# Link against CHAI3D and FreeGLUT libraries
target_link_libraries(aimlab-haptics ${CHAI3D_LIBRARIES} ${FREEGLUT_LIBRARIES})

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
