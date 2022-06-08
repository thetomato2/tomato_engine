@echo off
pushd "%~dp0"
cd ../../
if exist msvc-Debug rd /s /q msvc-Debug
cd tomato_engine
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
cmake --preset msvc-Debug
cd ../msvc-Debug
cmake --build .
devenv .\tomato_engine.exe
popd