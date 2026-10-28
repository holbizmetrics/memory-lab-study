@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b 1
set "MEMORYLAB_CMAKE=C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
"%MEMORYLAB_CMAKE%" -S "%~dp0." -B "%~dp0..\outputs\native-build" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
"%MEMORYLAB_CMAKE%" --build "%~dp0..\outputs\native-build"
if errorlevel 1 exit /b 1
"%~dp0..\outputs\native-build\memorylab_native.exe" --test
if errorlevel 1 exit /b 1
"%~dp0..\outputs\native-build\memorylab_native.exe" "%~dp0..\outputs\native-phrase.wav"
