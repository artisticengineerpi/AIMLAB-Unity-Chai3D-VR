##############################################################################
# AIMLAB-Unity-Chai3D-VR - Project Build Script
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.0
# 
# Description:
#   PowerShell script to build the AIMLAB haptic application.
#   This script assumes CHAI3D has already been built (run setup-chai3d.ps1 first).
#
# Usage:
#   .\build-project.ps1 [-Clean] [-Config <Debug|Release>]
#
# Parameters:
#   -Clean: Remove build directory before building
#   -Config: Build configuration (default: Release)
#
# Prerequisites:
#   - CHAI3D library already built (run setup-chai3d.ps1)
#   - CMake >= 3.10
#   - Visual Studio 2022 with C++ workload
#
# Changelog:
#   v1.0 - 04 February 2026 - Initial build script
#
##############################################################################

param(
    [switch]$Clean = $false,
    [ValidateSet('Debug', 'Release')]
    [string]$Config = 'Release'
)

# Enable strict error handling
$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "AIMLAB-Haptics Build Script" -ForegroundColor Cyan
Write-Host "Author: Pi Ko (pi.ko@nyu.edu)" -ForegroundColor Cyan
Write-Host "Date: 04 February 2026" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Get project root
$ProjectRoot = $PSScriptRoot
Write-Host "[INFO] Project root: $ProjectRoot" -ForegroundColor Green
Write-Host "[INFO] Build configuration: $Config" -ForegroundColor Green

# Check if CHAI3D is built
$CHAI3DLib = "$ProjectRoot\external\chai3d\build\Release\chai3d.lib"
if (-not (Test-Path $CHAI3DLib)) {
    Write-Host "[ERROR] CHAI3D library not found!" -ForegroundColor Red
    Write-Host "[ERROR] Expected: $CHAI3DLib" -ForegroundColor Red
    Write-Host "" -ForegroundColor Red
    Write-Host "Please run setup-chai3d.ps1 first to build CHAI3D:" -ForegroundColor Yellow
    Write-Host "  .\setup-chai3d.ps1" -ForegroundColor Yellow
    Write-Host ""
    exit 1
}

Write-Host "[SUCCESS] CHAI3D library found" -ForegroundColor Green

# Clean build if requested
if ($Clean -and (Test-Path "$ProjectRoot\build")) {
    Write-Host ""
    Write-Host "[INFO] Removing existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "$ProjectRoot\build"
    Write-Host "[SUCCESS] Build directory removed" -ForegroundColor Green
}

# Create build directory
if (-not (Test-Path "$ProjectRoot\build")) {
    Write-Host ""
    Write-Host "[STEP 1] Creating build directory..." -ForegroundColor Cyan
    New-Item -ItemType Directory -Path "$ProjectRoot\build" | Out-Null
    Write-Host "[SUCCESS] Build directory created" -ForegroundColor Green
}

# Configure with CMake
Write-Host ""
Write-Host "[STEP 2] Running CMake configuration..." -ForegroundColor Cyan
Set-Location "$ProjectRoot\build"

cmake .. -G "Visual Studio 17 2022" -A x64

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] CMake configuration failed!" -ForegroundColor Red
    Set-Location $ProjectRoot
    exit 1
}

Write-Host "[SUCCESS] CMake configuration complete" -ForegroundColor Green

# Build the project
Write-Host ""
Write-Host "[STEP 3] Building project ($Config mode)..." -ForegroundColor Cyan

$BuildStart = Get-Date
cmake --build . --config $Config

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Build failed!" -ForegroundColor Red
    Set-Location $ProjectRoot
    exit 1
}

$BuildEnd = Get-Date
$BuildDuration = ($BuildEnd - $BuildStart).TotalSeconds

Write-Host "[SUCCESS] Build completed in $([math]::Round($BuildDuration, 1)) seconds" -ForegroundColor Green

# Verify executable
$ExePath = "$ProjectRoot\build\bin\$Config\aimlab-haptics.exe"
if (Test-Path $ExePath) {
    $ExeSize = (Get-Item $ExePath).Length / 1KB
    Write-Host "[SUCCESS] Executable created: aimlab-haptics.exe ($([math]::Round($ExeSize, 1)) KB)" -ForegroundColor Green
} else {
    Write-Host "[WARNING] Executable not found at expected location" -ForegroundColor Yellow
}

# Return to project root
Set-Location $ProjectRoot

# Summary
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Build Complete!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Run the application:" -ForegroundColor Green
Write-Host "  cd build\bin\$Config" -ForegroundColor Gray
Write-Host "  .\aimlab-haptics.exe" -ForegroundColor Gray
Write-Host ""
Write-Host "Or from project root:" -ForegroundColor Green
Write-Host "  .\build\bin\$Config\aimlab-haptics.exe" -ForegroundColor Gray
Write-Host ""
Write-Host "Remember to connect and calibrate your Inverse3 device!" -ForegroundColor Yellow
Write-Host "See docs/SETUP_INSTRUCTIONS.md for device setup." -ForegroundColor Yellow
Write-Host ""
