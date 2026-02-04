##############################################################################
# AIMLAB-Unity-Chai3D-VR - Run Script with Process Cleanup
#
# Author: Pi Ko (pi.ko@nyu.edu)
# Date: 04 February 2026
# Version: v1.0
# 
# Description:
#   Automatically kills Haply processes that hold the COM port, then runs
#   the application. This resolves ACCESS_DENIED (error 5) serial port issues.
#
# Usage:
#   .\run.ps1
#
# What it does:
#   1. Kills haply-inverse-service (background service)
#   2. Kills Haply Hub UI if running
#   3. Stops Haply/Inverse Windows services
#   4. Launches aimlab-haptics.exe
#
# Changelog:
#   v1.0 - 04 February 2026 - Initial run script with process cleanup
#
##############################################################################

# Enable strict error handling
$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "AIMLAB-Haptics Launcher" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Get project root
$ProjectRoot = $PSScriptRoot
$ExePath = "$ProjectRoot\build\Release\aimlab-haptics.exe"

# Check if executable exists
if (-not (Test-Path $ExePath)) {
    Write-Host "[ERROR] Executable not found!" -ForegroundColor Red
    Write-Host "[ERROR] Expected: $ExePath" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please build the project first:" -ForegroundColor Yellow
    Write-Host "  .\rebuild.ps1" -ForegroundColor Yellow
    Write-Host ""
    exit 1
}

Write-Host "[INFO] Executable found: aimlab-haptics.exe" -ForegroundColor Green

# Step 1: Kill Haply processes
Write-Host ""
Write-Host "[STEP 1] Checking for Haply processes..." -ForegroundColor Cyan

$haplyProcesses = Get-Process | Where-Object { 
    $_.ProcessName -like "*haply*" -or 
    $_.ProcessName -like "*inverse*" 
}

if ($haplyProcesses) {
    Write-Host "[INFO] Found Haply processes:" -ForegroundColor Yellow
    $haplyProcesses | ForEach-Object { 
        Write-Host "  - $($_.ProcessName) (PID: $($_.Id))" -ForegroundColor Gray 
    }
    
    Write-Host "[ACTION] Killing Haply processes..." -ForegroundColor Yellow
    
    Stop-Process -Name "haply-inverse-service" -Force -ErrorAction SilentlyContinue
    Stop-Process -Name "Haply Hub" -Force -ErrorAction SilentlyContinue
    Stop-Process -Name "haply*" -Force -ErrorAction SilentlyContinue
    Stop-Process -Name "inverse*" -Force -ErrorAction SilentlyContinue
    
    Start-Sleep -Seconds 1  # Give processes time to terminate
    
    Write-Host "[SUCCESS] Haply processes terminated" -ForegroundColor Green
} else {
    Write-Host "[INFO] No Haply processes found" -ForegroundColor Gray
}

# Step 2: Stop Haply Windows services
Write-Host ""
Write-Host "[STEP 2] Checking for Haply services..." -ForegroundColor Cyan

$haplyServices = Get-Service -ErrorAction SilentlyContinue | Where-Object { 
    $_.DisplayName -like "*haply*" -or 
    $_.DisplayName -like "*inverse*" 
}

if ($haplyServices) {
    Write-Host "[INFO] Found Haply services:" -ForegroundColor Yellow
    $haplyServices | ForEach-Object { 
        Write-Host "  - $($_.DisplayName) ($($_.Status))" -ForegroundColor Gray 
    }
    
    Write-Host "[ACTION] Stopping Haply services..." -ForegroundColor Yellow
    
    $haplyServices | ForEach-Object {
        if ($_.Status -eq "Running") {
            Stop-Service -Name $_.Name -Force -ErrorAction SilentlyContinue
            Write-Host "  Stopped: $($_.DisplayName)" -ForegroundColor Green
        }
    }
    
    Write-Host "[SUCCESS] Haply services stopped" -ForegroundColor Green
} else {
    Write-Host "[INFO] No Haply services found" -ForegroundColor Gray
}

# Step 3: Launch application
Write-Host ""
Write-Host "[STEP 3] Launching application..." -ForegroundColor Cyan
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Starting aimlab-haptics.exe" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

# Change to build directory and run
Set-Location "$ProjectRoot\build\Release"
& ".\aimlab-haptics.exe"

# Return to project root after app closes
Set-Location $ProjectRoot

Write-Host ""
Write-Host "[INFO] Application exited" -ForegroundColor Gray
Write-Host ""
