##############################################################################
# AIMLAB-Unity-Chai3D-VR - Clean Rebuild Script
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.0
# 
# Description:
#   Convenience script to perform a complete clean rebuild of the project.
#   Deletes the build directory and rebuilds from scratch.
#
# Usage:
#   .\rebuild.ps1
#
# Prerequisites:
#   - CHAI3D library already built
#   - CMake 3.15+ installed
#   - Visual Studio 2022 with C++ workload
#
# Changelog:
#   v1.0 - 04 February 2026 - Initial rebuild script
#
##############################################################################

# Enable strict error handling
$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "AIMLAB-Haptics Clean Rebuild Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Get project root
$ProjectRoot = $PSScriptRoot
Write-Host "[INFO] Project root: $ProjectRoot" -ForegroundColor Green

# Check if build directory exists
if (Test-Path "$ProjectRoot\build") {
    Write-Host "[STEP 1] Removing existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "$ProjectRoot\build"
    Write-Host "[SUCCESS] Build directory removed" -ForegroundColor Green
} else {
    Write-Host "[INFO] No existing build directory found" -ForegroundColor Gray
}

# Create fresh build directory
Write-Host ""
Write-Host "[STEP 2] Creating fresh build directory..." -ForegroundColor Cyan
New-Item -ItemType Directory -Path "$ProjectRoot\build" | Out-Null
Write-Host "[SUCCESS] Build directory created" -ForegroundColor Green

# Navigate to build directory
Set-Location "$ProjectRoot\build"

# Configure with CMake
Write-Host ""
Write-Host "[STEP 3] Running CMake configuration..." -ForegroundColor Cyan
Write-Host "[INFO] Configuration: Release" -ForegroundColor Gray
Write-Host "[INFO] Generator: Visual Studio 17 2022" -ForegroundColor Gray
Write-Host "[INFO] Architecture: x64" -ForegroundColor Gray

cmake .. -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[ERROR] CMake configuration failed!" -ForegroundColor Red
    Set-Location $ProjectRoot
    exit 1
}

Write-Host "[SUCCESS] CMake configuration complete" -ForegroundColor Green

# Build the project
Write-Host ""
Write-Host "[STEP 4] Building project (Release mode)..." -ForegroundColor Cyan
Write-Host "[INFO] This may take 30-60 seconds..." -ForegroundColor Yellow

$BuildStart = Get-Date
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[ERROR] Build failed!" -ForegroundColor Red
    Set-Location $ProjectRoot
    exit 1
}

$BuildEnd = Get-Date
$BuildDuration = ($BuildEnd - $BuildStart).TotalSeconds

Write-Host "[SUCCESS] Build completed in $([math]::Round($BuildDuration, 1)) seconds" -ForegroundColor Green

# Verify executable
$ExePath = "$ProjectRoot\build\Release\aimlab-haptics.exe"
if (Test-Path $ExePath) {
    $ExeSize = (Get-Item $ExePath).Length / 1KB
    Write-Host "[SUCCESS] Executable created: aimlab-haptics.exe ($([math]::Round($ExeSize, 1)) KB)" -ForegroundColor Green
} else {
    Write-Host "[WARNING] Executable not found at expected location" -ForegroundColor Yellow
}

# Return to project root
Set-Location $ProjectRoot

# Return to project root
Set-Location $ProjectRoot

# Summary
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Rebuild Complete!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Ask if user wants to run the app
$response = Read-Host "Do you want to run the application now? (Y/n)"

if ($response -eq '' -or $response -eq 'y' -or $response -eq 'Y') {
    Write-Host ""
    Write-Host "[INFO] Launching application with process cleanup..." -ForegroundColor Cyan
    & "$ProjectRoot\run.ps1"
} else {
    Write-Host ""
    Write-Host "Run the application later with:" -ForegroundColor Green
    Write-Host "  .\run.ps1" -ForegroundColor White
    Write-Host ""
    Write-Host "Or manually:" -ForegroundColor Green
    Write-Host "  .\build\Release\aimlab-haptics.exe" -ForegroundColor Gray
    Write-Host ""
    Write-Host "The app runs in VISUAL-ONLY mode if no device detected." -ForegroundColor Yellow
    Write-Host "To enable haptics: Use .\run.ps1 (kills Haply Hub automatically)" -ForegroundColor Yellow
    Write-Host ""
}
