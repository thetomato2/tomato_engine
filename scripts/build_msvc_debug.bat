@echo off
pushd "%~dp0"
cd ../../msvc-Debug
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
cmake --build .
popd