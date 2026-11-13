@echo off
cd /d "%~dp0"
echo ========================================================
echo Building Food Delivery System (Qt Graphical Application)
echo ========================================================

where cmake >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [1/3] Configuring with CMake...
    where ninja >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        cmake -B build -G Ninja
    ) else (
        cmake -B build
    )
    if %ERRORLEVEL% NEQ 0 (
        echo CMake configuration failed!
        exit /b 1
    )
    echo [2/3] Compiling target FoodDeliveryApp...
    cmake --build build --target FoodDeliveryApp
    if %ERRORLEVEL% EQU 0 (
        echo [3/3] Build successful! Starting application...
        cd build
        start "" FoodDeliveryApp.exe
    )
    goto :eof
)

where qmake >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    where mingw32-make >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        echo [1/3] Running qmake on DsProject.pro...
        qmake DsProject.pro
        echo [2/3] Building with mingw32-make...
        mingw32-make -j4
        if %ERRORLEVEL% EQU 0 (
            echo [3/3] Build successful! Starting application...
            if exist release\FoodDeliveryApp.exe (
                start "" release\FoodDeliveryApp.exe
            ) else if exist FoodDeliveryApp.exe (
                start "" FoodDeliveryApp.exe
            )
        )
        goto :eof
    )
)

echo [Notice] Required build tools were not found.
pause
