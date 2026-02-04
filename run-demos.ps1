##############################################################################
# AIMLAB-Unity-Chai3D-VR - CHAI3D Demo Runner Script
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.0
# 
# Description:
#   PowerShell script to launch CHAI3D demo applications. Provides an
#   interactive menu to select and run any of the 31 built-in demos.
#
# Usage:
#   .\run-demos.ps1
#
# Prerequisites:
#   - CHAI3D library built (run setup-chai3d.ps1)
#   - Inverse3 device connected and calibrated
#   - Haply Hub / Inverse Service running
#
# Changelog:
#   v1.0 - 04 February 2026 - Initial demo runner script
#
##############################################################################

# Enable strict error handling
$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CHAI3D Demo Runner" -ForegroundColor Cyan
Write-Host "Author: Pi Ko (pi.ko@nyu.edu)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Get project root
$ProjectRoot = $PSScriptRoot
$DemoPath = "$ProjectRoot\external\chai3d\build\bin\Release"

# Check if demos exist
if (-not (Test-Path $DemoPath)) {
    Write-Host "[ERROR] Demo directory not found!" -ForegroundColor Red
    Write-Host "[ERROR] Expected: $DemoPath" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please build CHAI3D first:" -ForegroundColor Yellow
    Write-Host "  .\setup-chai3d.ps1" -ForegroundColor Yellow
    Write-Host ""
    exit 1
}

# Get all demo executables
$Demos = Get-ChildItem "$DemoPath\*.exe" | Sort-Object Name

if ($Demos.Count -eq 0) {
    Write-Host "[ERROR] No demo executables found in $DemoPath" -ForegroundColor Red
    exit 1
}

Write-Host "[SUCCESS] Found $($Demos.Count) demo applications" -ForegroundColor Green
Write-Host ""

# Pre-flight checklist
Write-Host "PRE-FLIGHT CHECKLIST:" -ForegroundColor Yellow
Write-Host "  [?] Inverse3 mounted and arms unfolded?" -ForegroundColor Gray
Write-Host "  [?] USB-C connected (purple LED)?" -ForegroundColor Gray
Write-Host "  [?] 24V power connected?" -ForegroundColor Gray
Write-Host "  [?] Device calibrated (white LED)?" -ForegroundColor Gray
Write-Host "  [?] Haply Hub running?" -ForegroundColor Gray
Write-Host ""

$response = Read-Host "Is device ready? (Y/n)"
if ($response -eq 'n' -or $response -eq 'N') {
    Write-Host "[INFO] Please prepare your device first." -ForegroundColor Yellow
    Write-Host "See docs/SETUP_INSTRUCTIONS.md section 3 for help." -ForegroundColor Yellow
    exit 0
}

# Display demo menu
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Available Demos:" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Recommended demos with descriptions
$RecommendedDemos = @{
    "01-mydevice.exe" = "Basic device test - read position, send forces"
    "04-shapes.exe" = "Basic 3D shapes with haptic interaction"
    "11-effects.exe" = "Surface effects (vibration, friction, viscosity)"
    "13-primitives.exe" = "Geometric primitives (sphere, box, cylinder)"
    "21-object.exe" = "Load and interact with 3D models"
    "23-tooth.exe" = "Dental simulation (tooth drilling)"
}

Write-Host "RECOMMENDED DEMOS:" -ForegroundColor Green
$index = 1
foreach ($demo in $RecommendedDemos.GetEnumerator() | Sort-Object Key) {
    Write-Host "  [$index] $($demo.Key)" -ForegroundColor White
    Write-Host "      $($demo.Value)" -ForegroundColor Gray
    $index++
}

Write-Host ""
Write-Host "ALL DEMOS:" -ForegroundColor Yellow
$index = 1
foreach ($demo in $Demos) {
    Write-Host "  [$index] $($demo.Name)" -ForegroundColor White
    $index++
}

Write-Host ""
Write-Host "  [0] Exit" -ForegroundColor Red
Write-Host ""

# Get user selection
do {
    $selection = Read-Host "Select demo number (1-$($Demos.Count))"
    
    if ($selection -eq '0') {
        Write-Host "[INFO] Exiting..." -ForegroundColor Green
        exit 0
    }
    
    $selectionNum = [int]$selection
    
    if ($selectionNum -lt 1 -or $selectionNum -gt $Demos.Count) {
        Write-Host "[ERROR] Invalid selection. Please choose 1-$($Demos.Count)" -ForegroundColor Red
        continue
    }
    
    # Run selected demo
    $SelectedDemo = $Demos[$selectionNum - 1]
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Launching: $($SelectedDemo.Name)" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Controls:" -ForegroundColor Yellow
    Write-Host "  ESC - Quit" -ForegroundColor Gray
    Write-Host "  f - Fullscreen" -ForegroundColor Gray
    Write-Host "  Mouse drag - Rotate view" -ForegroundColor Gray
    Write-Host ""
    
    Set-Location $DemoPath
    & ".\$($SelectedDemo.Name)"
    
    Set-Location $ProjectRoot
    
    Write-Host ""
    $again = Read-Host "Run another demo? (Y/n)"
    if ($again -eq 'n' -or $again -eq 'N') {
        break
    }
    
} while ($true)

Write-Host ""
Write-Host "[INFO] Demo runner exiting. Goodbye!" -ForegroundColor Green
Write-Host ""
