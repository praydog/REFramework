git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64
cd build64
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=OFF
cmake --build . --config Release