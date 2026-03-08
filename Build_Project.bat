@echo off

echo "creating build directory"
mkdir build
cd build

echo "Initializing Cmake"
cmake ..

echo "Building DLL"
cmake --build . --config Release
