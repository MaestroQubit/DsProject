# Food Delivery System - Qt Build & Run Script
Set-Location $PSScriptRoot

Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "Food Delivery System (Qt Graphical Application)" -ForegroundColor Cyan
Write-Host "========================================================`n" -ForegroundColor Cyan

$cmakeCmd = Get-Command cmake -ErrorAction SilentlyContinue
$ninjaCmd = Get-Command ninja -ErrorAction SilentlyContinue

if ($cmakeCmd) {
    Write-Host "[1/3] Configuring with CMake..." -ForegroundColor Yellow
    if ($ninjaCmd) {
        & cmake -B build -G Ninja
    } else {
        & cmake -B build
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "CMake configuration failed." -ForegroundColor Red
        exit $LASTEXITCODE
    }

    Write-Host "[2/3] Building target FoodDeliveryApp..." -ForegroundColor Yellow
    & cmake --build build --target FoodDeliveryApp
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed." -ForegroundColor Red
        exit $LASTEXITCODE
    }

    Write-Host "[3/3] Build successful! Launching graphical window...`n" -ForegroundColor Green
    $exePath = Join-Path $PSScriptRoot "build\FoodDeliveryApp.exe"
    if (Test-Path $exePath) {
        Start-Process $exePath -WorkingDirectory (Join-Path $PSScriptRoot "build")
    }
    exit 0
}

$qmakeCmd = Get-Command qmake -ErrorAction SilentlyContinue
$makeCmd = Get-Command mingw32-make, make -ErrorAction SilentlyContinue | Select-Object -First 1

if ($qmakeCmd -and $makeCmd) {
    Write-Host "[1/3] Running qmake on DsProject.pro..." -ForegroundColor Yellow
    & qmake DsProject.pro
    if ($LASTEXITCODE -ne 0) {
        Write-Host "qmake configuration failed." -ForegroundColor Red
        exit $LASTEXITCODE
    }

    Write-Host "[2/3] Building application with $($makeCmd.Name)..." -ForegroundColor Yellow
    & $makeCmd.Name -j4
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[3/3] Build successful! Launching graphical window...`n" -ForegroundColor Green
        if (Test-Path "release\FoodDeliveryApp.exe") {
            Start-Process "release\FoodDeliveryApp.exe"
        } elseif (Test-Path "FoodDeliveryApp.exe") {
            Start-Process "FoodDeliveryApp.exe"
        }
    }
    exit 0
}

Write-Host "[Notice] Required build tools were not found." -ForegroundColor Yellow
Write-Host "Please ensure CMake and Ninja or MinGW are installed." -ForegroundColor White
