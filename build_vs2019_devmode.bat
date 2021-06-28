git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64
cd build64
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE2=ON
cmake --build . --config Release

cd ..
mkdir build64_re3
cd build64_re3
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE3=ON
cmake --build . --config Release

cd ..
mkdir build64_re8
cd build64_re8
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE8=ON
cmake --build . --config Release

cd ..
mkdir build64_dmc5
cd build64_dmc5
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DDMC5=ON
cmake --build . --config Release