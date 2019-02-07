git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64
cd build64
cmake .. -G "Visual Studio 15 2017 Win64" -DDEVELOPER_MODE=ON
cmake --build . --config Release