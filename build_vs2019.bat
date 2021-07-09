git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64_all
cd build64_all
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=OFF -DRE2=ON
cmake --build . --config Release
