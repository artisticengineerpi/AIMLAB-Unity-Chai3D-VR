##############################################################################
# AIMLAB-Unity-Chai3D-VR - Official CHAI3D Demo Launcher
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.0
# 
# Description:
#   Launches official Haply CHAI3D demos (v3.3.5) with proper Inverse3 support.
#   These demos use Inverse SDK and work WITH Haply Hub running.
#
# Usage:
#   .\run-official-demos.ps1
#
# Prerequisites:
#   - Official CHAI3D v3.3.5 downloaded and extracted
#   - Haply Hub running with Inverse Service
#   - Inverse3 connected and calibrated
#
# Changelog:
#   v1.0 - 04 February 2026 - Initial official demo launcher
#
##############################################################################

# Enable strict error handling
$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Official CHAI3D Demo Launcher (v3.3.5)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$ProjectRoot = $PSScriptRoot
$DemoPath = "$ProjectRoot\Chai3D.3.3.5\win-x64"

# Check if official CHAI3D is downloaded
if (-not (Test-Path $DemoPath)) {
    Write-Host "[ERROR] Official CHAI3D demos not found!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please download CHAI3D v3.3.5:" -ForegroundColor Yellow
    Write-Host "  1. Visit: https://develop.haply.co/releases/chai3d" -ForegroundColor White
    Write-Host "  2. Download: Chai3D.3.3.5.zip (Windows)" -ForegroundColor White
    Write-Host "  3. Extract to: $ProjectRoot\Chai3D.3.3.5\" -ForegroundColor White
    Write-Host ""
    exit 1
}

# Get all demo executables
$Demos = Get-ChildItem "$DemoPath\*.exe" | Sort-Object Name

Write-Host "[SUCCESS] Found $($Demos.Count) official CHAI3D demos" -ForegroundColor Green
Write-Host ""

# Pre-flight checklist for Inverse3
Write-Host "PRE-FLIGHT CHECKLIST (Inverse3):" -ForegroundColor Yellow
Write-Host "  [?] Haply Hub is RUNNING (required for Inverse SDK)" -ForegroundColor Gray
Write-Host "  [?] Inverse3 connected via USB-C (purple LED)?" -ForegroundColor Gray
Write-Host "  [?] 24V power connected?" -ForegroundColor Gray
Write-Host "  [?] Device calibrated (white LED)?" -ForegroundColor Gray
Write-Host ""
Write-Host "NOTE: Official demos use Inverse SDK, so Haply Hub MUST be running!" -ForegroundColor Cyan
Write-Host "      (Unlike the GitHub fork which requires Hub to be closed)" -ForegroundColor Cyan
Write-Host ""

$response = Read-Host "Is everything ready? (Y/n)"
if ($response -eq 'n' -or $response -eq 'N') {
    Write-Host "[INFO] Please prepare device first." -ForegroundColor Yellow
    exit 0
}

# Categorize demos
$StandardDemos = $Demos | Where-Object { $_.Name -match '^\d+-' -and $_.Name -notmatch 'GEL' }
$GELDemos = $Demos | Where-Object { $_.Name -match 'GEL' }
$UtilityDemos = $Demos | Where-Object { $_.Name -match '^c' }

# Display menu
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Demo Categories:" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "HAPLY-SPECIFIC DEMOS (Inverse3):" -ForegroundColor Magenta
$index = 1
foreach ($demo in $GELDemos) {
    $name = $demo.BaseName
    $description = switch ($name) {
        "00-GEL-haply" { "Haply Inverse3 demonstration" }
        "01-GEL-membrane" { "Puncture deformable membrane" }
        "02-GEL-cell" { "Cell deformation simulation" }
        "03-GEL-duck" { "Deformable duck model" }
        default { "GEL module demo" }
    }
    Write-Host "  [$index] $($demo.Name)" -ForegroundColor White
    Write-Host "      $description" -ForegroundColor Gray
    $index++
}

Write-Host ""
Write-Host "RECOMMENDED STANDARD DEMOS:" -ForegroundColor Green
$recommended = @{
    "01-mydevice.exe" = "Basic device test - verify Inverse3 connection"
    "04-shapes.exe" = "Basic 3D shapes with haptic interaction"
    "13-primitives.exe" = "Geometric primitives (popular starting demo)"
    "23-tooth.exe" = "Dental simulation (tooth drilling)"
}

foreach ($demo in $recommended.GetEnumerator() | Sort-Object Key) {
    Write-Host "  [$index] $($demo.Key)" -ForegroundColor White
    Write-Host "      $($demo.Value)" -ForegroundColor Gray
    $index++
}

Write-Host ""
Write-Host "ALL DEMOS ($($Demos.Count) total):" -ForegroundColor Yellow
Write-Host "  Type 'list' to see all demos" -ForegroundColor Gray
Write-Host ""
Write-Host "  [0] Exit" -ForegroundColor Red
Write-Host ""

# Get user selection
do {
    $selection = Read-Host "Select demo number, name, or 'list'"
    
    if ($selection -eq '0') {
        Write-Host "[INFO] Exiting..." -ForegroundColor Green
        exit 0
    }
    
    if ($selection -eq 'list') {
        Write-Host ""
        Write-Host "All Available Demos:" -ForegroundColor Cyan
        $demoIndex = 1
        foreach ($demo in $Demos) {
            Write-Host "  [$demoIndex] $($demo.Name)" -ForegroundColor White
            $demoIndex++
        }
        Write-Host ""
        continue
    }
    
    # Try parsing as number
    if ($selection -match '^\d+$') {
        $selectionNum = [int]$selection
        
        if ($selectionNum -lt 1 -or $selectionNum -gt $Demos.Count) {
            Write-Host "[ERROR] Invalid selection. Please choose 1-$($Demos.Count)" -ForegroundColor Red
            continue
        }
        
        $SelectedDemo = $Demos[$selectionNum - 1]
    }
    # Try matching by name
    else {
        $SelectedDemo = $Demos | Where-Object { $_.Name -like "*$selection*" } | Select-Object -First 1
        
        if (-not $SelectedDemo) {
            Write-Host "[ERROR] No demo found matching '$selection'" -ForegroundColor Red
            continue
        }
    }
    
    # Run selected demo
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Launching: $($SelectedDemo.Name)" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Controls:" -ForegroundColor Yellow
    Write-Host "  ESC or 'q' - Quit" -ForegroundColor Gray
    Write-Host "  'f' - Fullscreen" -ForegroundColor Gray
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
Write-Host "[INFO] Demo launcher exiting. Goodbye!" -ForegroundColor Green
Write-Host ""
