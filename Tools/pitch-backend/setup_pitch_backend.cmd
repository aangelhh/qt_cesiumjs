@echo off
setlocal EnableExtensions

set "HERE=%~dp0"
set "SOURCE=%HERE%pitch-backend-source"
set "BUILD=%HERE%pitch-backend-build"
set "PLUGIN_DIR=%HERE%hla-plugins"

if not defined PitchRTI_ROOT (
  if exist "C:\Program Files\Pitch\prti1516e\cmake" set "PitchRTI_ROOT=C:\Program Files\Pitch\prti1516e"
)
if not defined PitchRTI_ROOT (
  if exist "C:\Program Files\prti1516e\cmake" set "PitchRTI_ROOT=C:\Program Files\prti1516e"
)
if not defined PitchRTI_ROOT (
  if exist "C:\Pitch\prti1516e\cmake" set "PitchRTI_ROOT=C:\Pitch\prti1516e"
)

if not defined PitchRTI_ROOT (
  echo.
  echo Pitch pRTI SDK was not found automatically.
  echo Set PitchRTI_ROOT to the pRTI installation that contains cmake\PitchRTIConfig.cmake,
  echo then run this script again.
  echo Example:
  echo   set PitchRTI_ROOT=C:\Program Files\Pitch\prti1516e
  exit /b 2
)

where cmake >nul 2>nul
if errorlevel 1 (
  echo CMake is required to build the qttest Pitch adapter.
  exit /b 3
)

if not exist "%SOURCE%\CMakeLists.txt" (
  echo Packaged Pitch backend source is missing: %SOURCE%
  exit /b 4
)

echo Building qttest Pitch backend with:
echo   PitchRTI_ROOT=%PitchRTI_ROOT%

cmake -S "%SOURCE%" -B "%BUILD%" -A x64 -DPitchRTI_ROOT="%PitchRTI_ROOT%"
if errorlevel 1 exit /b 5

cmake --build "%BUILD%" --config Release --parallel
if errorlevel 1 exit /b 6

if not exist "%BUILD%\out\qttest_hla_pitch1516e_backend.dll" (
  echo The adapter DLL was not generated.
  exit /b 7
)

if not exist "%PLUGIN_DIR%" mkdir "%PLUGIN_DIR%"
copy /Y "%BUILD%\out\qttest_hla_pitch1516e_backend.dll" "%PLUGIN_DIR%\qttest_hla_pitch1516e_backend.dll" >nul
if errorlevel 1 exit /b 8

echo.
echo Pitch adapter installed successfully:
echo   %PLUGIN_DIR%\qttest_hla_pitch1516e_backend.dll
echo.
echo Start qttest again. Pitch pRTI should now appear in the HLA backend list.
exit /b 0
