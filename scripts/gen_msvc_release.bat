@echo off
pushd "%~dp0"
cd ../../
if exist msvc-Release rd /s /q msvc-Release
cd tomato_engine
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
cmake --preset msvc-Release
cd ../msvc-Release
cmake --build .
popd