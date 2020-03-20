git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64
cd build64
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON
cmake --build . --config Release

cd ..
mkdir build64_re3
cd build64_re3
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE3=ON
cmake --build . --config Release