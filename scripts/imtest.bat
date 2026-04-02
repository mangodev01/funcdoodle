@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "root_dir=%~dp0.."
set "root_dir=%root_dir:\=%"

if "%~1"=="" (
    set "mode=Debug"
    set "clean=false"
) else (
    if "%~2"=="" (
        echo Usage: %~nx0 [Debug|Release] [clean=true^|false]
        exit /b 1
    )
    set "mode=%~1"
    set "clean=%~2"
)

if /I "%mode%"=="debug" (
    set "mode=Debug"
) else (
    if /I "%mode%"=="release" (
        set "mode=Release"
    ) else (
        echo Mode must be Debug or Release -- got '%mode%'
        exit /b 1
    )
)

if /I not "%clean%"=="true" (
    if /I not "%clean%"=="false" (
        echo clean must be true or false -- got '%clean%'
        exit /b 1
    )
)

set "bin_dir=%root_dir%\bin\microslop"

if /I "%clean%"=="true" (
    if exist "%bin_dir%" rmdir /S /Q "%bin_dir%"
)

mkdir "%bin_dir%"
if errorlevel 1 exit /b 1

cd "%bin_dir%"
if errorlevel 1 exit /b 1

cmake -DCMAKE_BUILD_TYPE=%mode% -DISTILING=ON -DBUILD_IMTESTS=ON "%root_dir%"
if errorlevel 1 exit /b 1

cmake --build . --config %mode% --parallel
if errorlevel 1 exit /b 1

xcopy /E /I /Y "%root_dir%\assets" ".\assets" >nul
if errorlevel 1 exit /b 1

xcopy /E /I /Y "%root_dir%\themes" ".\themes" >nul
if errorlevel 1 exit /b 1

if exist ".\%mode%\FuncDoodle.exe" (
    .\FuncDoodle.exe
) else (
    .\FuncDoodle.exe
)
exit /b %errorlevel%