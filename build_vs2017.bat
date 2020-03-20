git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64
cd build64
cmake .. -G "Visual Studio 15 2017 Win64" -DDEVELOPER_MODE=OFF
cmake --build . --config Release

cd ..
mkdir build64_re3
cd build64_re3
cmake .. -G "Visual Studio 15 2017 Win64" -DDEVELOPER_MODE=OFF -DRE3=ON
cmake --build . --config Release