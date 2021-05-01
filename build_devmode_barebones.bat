git pull --recurse-submodules
git submodule update --init --recursive

mkdir build64_re8_barebones
cd build64_re8_barebones
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE3=ON -DRE8=ON -DBAREBONES=ON
cmake --build . --config Release