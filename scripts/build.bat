@echo off
setlocal EnableExtensions

set "root_dir=%~dp0.."
set "root_dir=%root_dir:\=%"

if "%~1"=="" (
    if not "%~2"=="" goto usage
    if not "%~3"=="" goto usage
    if not "%~4"=="" goto usage
    set "arg1=Debug"
    set "arg2=true"
    set "arg3=false"
    set "arg4=true"
) else (
    if not "%~5"=="" goto usage
    if "%~4"=="" goto usage
    set "arg1=%~1"
    set "arg2=%~2"
    set "arg3=%~3"
    set "arg4=%~4"
)

if /I "%arg1%"=="debug" (
    set "arg1=Debug"
) else (
    if /I "%arg1%"=="release" (
        set "arg1=Release"
    ) else (
        echo Mode must be either debug or release -- %arg1% is invalid
        exit /b -1
    )
)

call :normalize_bool "%arg2%" arg2
if errorlevel 1 (
    echo istiling argument must be true or false -- %arg2% is invalid
    exit /b -1
)

call :normalize_bool "%arg3%" arg3
if errorlevel 1 (
    echo clean argument must be true or false -- %arg3% is invalid
    exit /b -1
)

call :normalize_bool "%arg4%" arg4
if errorlevel 1 (
    echo run argument must be true or false -- %arg4% is invalid
    exit /b -1
)

set "bin_dir=%root_dir%\bin\microslop"

if "%arg3%"=="true" (
    if exist "%bin_dir%" rmdir /S /Q "%bin_dir%"
)

mkdir "%bin_dir%"
if errorlevel 1 exit /b -1

cd "%bin_dir%"
if errorlevel 1 exit /b -1

set "tiling_flag=OFF"
if "%arg2%"=="true" set "tiling_flag=ON"

set "use_bundled=%FUNCDOODLE_USE_BUNDLED_PORTAUDIO%"
if not defined use_bundled (
    if exist "%root_dir%\lib\portaudio\CMakeLists.txt" (
        set "use_bundled=ON"
    )
)

set "cmake_args=-DCMAKE_BUILD_TYPE=%arg1% -DISTILING=%tiling_flag% -DBUILD_TESTS=OFF -DBUILD_IMTESTS=OFF"

if "%use_bundled%"=="ON" (
    set "cmake_args=%cmake_args% -DFUNCDOODLE_USE_BUNDLED_PORTAUDIO=ON"
) else (
    set "cmake_args=%cmake_args% -DFUNCDOODLE_USE_BUNDLED_PORTAUDIO=OFF -DPORTAUDIO_STATIC=ON"
)

cmake %cmake_args% "%root_dir%"
if errorlevel 1 exit /b -1

cmake --build . --config %arg1% --parallel
if errorlevel 1 exit /b -1

xcopy /E /I /Y "%root_dir%\assets" ".\assets" >nul
if errorlevel 1 exit /b -1

xcopy /E /I /Y "%root_dir%\themes" ".\themes" >nul
if errorlevel 1 exit /b -1

if exist ".\%arg1%\" (
    xcopy /E /I /Y "%root_dir%\assets" ".\%arg1%\assets" >nul
    if errorlevel 1 exit /b -1
    xcopy /E /I /Y "%root_dir%\themes" ".\%arg1%\themes" >nul
    if errorlevel 1 exit /b -1
)

if "%arg4%"=="true" (
    if exist ".\FuncDoodle.exe" (
        .\FuncDoodle.exe
        if errorlevel 1 exit /b -1
    ) else (
        if exist ".\%arg1%\FuncDoodle.exe" (
            .\%arg1%\FuncDoodle.exe
            if errorlevel 1 exit /b -1
        ) else (
            echo Failed to find FuncDoodle.exe after build.
            exit /b -1
        )
    )
)

exit /b 0

:usage
echo Usage: %~nx0 ^<Debug/Release^> ^<tiling?^> ^<clean?^> ^<run?^>
exit /b -1

:normalize_bool
if /I "%~1"=="true" (
    set "%~2=true"
    exit /b 0
)
if /I "%~1"=="false" (
    set "%~2=false"
    exit /b 0
)
exit /b 1