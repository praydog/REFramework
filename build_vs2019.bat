git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64_all
cd build64_all
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=OFF
::cmake .. -G "Visual Studio 17 2022" -A x64 -DDEVELOPER_MODE=OFF
cmake --build . --config Release
