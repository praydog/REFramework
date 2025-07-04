@echo off
echo Building REFramework for MHRISE, DD2 and MHWILDS only...
echo.

:: Update submodules
git submodule update --init --recursive

:: Create build directory
if not exist build64_selected mkdir build64_selected
cd build64_selected

:: Configure CMake
echo Configuring CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DDEVELOPER_MODE=ON

:: Build only the selected targets
echo.
echo Building MHRISE...
cmake --build . --config Release --target MHRISE

echo.
echo Building DD2...
cmake --build . --config Release --target DD2

echo.
echo Building MHWILDS...
cmake --build . --config Release --target MHWILDS

echo.
echo Build complete! The DLLs can be found in:
echo - build64_selected\bin\MHRISE\dinput8.dll
echo - build64_selected\bin\DD2\dinput8.dll
echo - build64_selected\bin\MHWILDS\dinput8.dll
echo.
pause