##############################################################################
# AIMLAB-Unity-Chai3D-VR - CHAI3D Setup Automation Script
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.0
# 
# Description:
#   PowerShell script to automate CHAI3D library setup, including cloning
#   the Haply fork as a git submodule, checking out the correct branch,
#   and building the library with Haply device support enabled.
#
# Usage:
#   .\setup-chai3d.ps1
#
# Prerequisites:
#   - Git installed and in PATH
#   - CMake >= 3.10 installed
#   - Visual Studio 2022 with C++ workload
#   - Run from Developer PowerShell for VS 2022
#
# Changelog:
#   v1.0 - 04 February 2026 - Initial automation script
#
##############################################################################

# Enable strict error handling
$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CHAI3D Setup Script" -ForegroundColor Cyan
Write-Host "Author: Pi Ko (pi.ko@nyu.edu)" -ForegroundColor Cyan
Write-Host "Date: 04 February 2026" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Get project root directory
$ProjectRoot = $PSScriptRoot
Write-Host "[INFO] Project root: $ProjectRoot" -ForegroundColor Green

# Check if external/chai3d already exists
if (Test-Path "$ProjectRoot\external\chai3d") {
    Write-Host "[WARNING] external/chai3d already exists!" -ForegroundColor Yellow
    $response = Read-Host "Do you want to remove and re-clone? (y/N)"
    
    if ($response -eq 'y' -or $response -eq 'Y') {
        Write-Host "[INFO] Removing existing chai3d directory..." -ForegroundColor Green
        Remove-Item -Recurse -Force "$ProjectRoot\external\chai3d"
    } else {
        Write-Host "[INFO] Using existing chai3d directory." -ForegroundColor Green
        Set-Location "$ProjectRoot\external\chai3d"
        
        # Update submodules
        Write-Host "[INFO] Updating submodules..." -ForegroundColor Green
        git submodule update --init --recursive
        
        Set-Location $ProjectRoot
        $SkipClone = $true
    }
}

# Step 1: Add CHAI3D as git submodule
if (-not $SkipClone) {
    Write-Host ""
    Write-Host "[STEP 1] Adding CHAI3D as git submodule..." -ForegroundColor Cyan
    
    try {
        git submodule add https://github.com/HaplyHaptics/chai3d.git external/chai3d
        Write-Host "[SUCCESS] CHAI3D submodule added" -ForegroundColor Green
    } catch {
        Write-Host "[ERROR] Failed to add submodule. It may already exist." -ForegroundColor Red
    }

    # Checkout the haply-api-cpp branch
    Write-Host ""
    Write-Host "[STEP 2] Checking out haply-api-cpp branch..." -ForegroundColor Cyan
    Set-Location "$ProjectRoot\external\chai3d"
    git checkout haply-api-cpp
    Write-Host "[SUCCESS] Checked out haply-api-cpp branch" -ForegroundColor Green

    # Initialize nested submodules (Haply-API-cpp)
    Write-Host ""
    Write-Host "[STEP 3] Initializing nested submodules..." -ForegroundColor Cyan
    git submodule update --init --recursive
    Write-Host "[SUCCESS] Submodules initialized" -ForegroundColor Green

    Set-Location $ProjectRoot
}

# Step 2: Build CHAI3D library
Write-Host ""
Write-Host "[STEP 4] Building CHAI3D library..." -ForegroundColor Cyan
Set-Location "$ProjectRoot\external\chai3d"

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
    Write-Host "[INFO] Created build directory" -ForegroundColor Green
} else {
    Write-Host "[INFO] Build directory already exists" -ForegroundColor Green
}

Set-Location "build"

# Configure with CMake
Write-Host "[INFO] Running CMake configuration..." -ForegroundColor Green
Write-Host "[INFO] This may take a few minutes..." -ForegroundColor Yellow
cmake .. -G "Visual Studio 17 2022" -A x64 -DENABLE_HAPLY_DEVICES=ON

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] CMake configuration failed!" -ForegroundColor Red
    Write-Host "[HINT] Make sure you're running from Developer PowerShell for VS 2022" -ForegroundColor Yellow
    Set-Location $ProjectRoot
    exit 1
}

Write-Host "[SUCCESS] CMake configuration complete" -ForegroundColor Green

# Build the library
Write-Host ""
Write-Host "[INFO] Building CHAI3D (Release mode)..." -ForegroundColor Green
Write-Host "[INFO] This will take 5-15 minutes depending on your system..." -ForegroundColor Yellow
Write-Host "[INFO] Building all targets (library + 31 demos + modules)..." -ForegroundColor Yellow

$BuildStart = Get-Date
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Build failed!" -ForegroundColor Red
    Set-Location $ProjectRoot
    exit 1
}

$BuildEnd = Get-Date
$BuildDuration = ($BuildEnd - $BuildStart).TotalMinutes

Write-Host "[SUCCESS] CHAI3D built successfully!" -ForegroundColor Green
Write-Host "[INFO] Build time: $([math]::Round($BuildDuration, 1)) minutes" -ForegroundColor Green

# Return to project root
Set-Location $ProjectRoot

# Step 3: Verify build
Write-Host ""
Write-Host "[STEP 5] Verifying build output..." -ForegroundColor Cyan

$LibraryPath = "$ProjectRoot\external\chai3d\build\Release\chai3d.lib"
if (Test-Path $LibraryPath) {
    $LibSize = (Get-Item $LibraryPath).Length / 1MB
    Write-Host "[SUCCESS] Library found: chai3d.lib ($([math]::Round($LibSize, 1)) MB)" -ForegroundColor Green
} else {
    Write-Host "[WARNING] Library not found at expected location" -ForegroundColor Yellow
}

$DemoPath = "$ProjectRoot\external\chai3d\build\bin\Release"
if (Test-Path $DemoPath) {
    $DemoCount = (Get-ChildItem "$DemoPath\*.exe" | Measure-Object).Count
    Write-Host "[SUCCESS] Found $DemoCount demo executables" -ForegroundColor Green
} else {
    Write-Host "[WARNING] Demo executables not found" -ForegroundColor Yellow
}

# Summary
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CHAI3D Setup Complete!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Green
Write-Host "1. Build your application:" -ForegroundColor White
Write-Host "   mkdir build" -ForegroundColor Gray
Write-Host "   cd build" -ForegroundColor Gray
Write-Host "   cmake .. -G 'Visual Studio 17 2022' -A x64" -ForegroundColor Gray
Write-Host "   cmake --build . --config Release" -ForegroundColor Gray
Write-Host ""
Write-Host "2. Or run CHAI3D demos:" -ForegroundColor White
Write-Host "   cd external\chai3d\build\bin\Release" -ForegroundColor Gray
Write-Host "   .\01-mydevice.exe" -ForegroundColor Gray
Write-Host "   .\13-primitives.exe" -ForegroundColor Gray
Write-Host ""
Write-Host "See docs/BUILD_GUIDE.md for more information." -ForegroundColor Green
Write-Host ""
