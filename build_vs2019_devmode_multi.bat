git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64_re2
cd build64_re2
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE2=ON -DSINGLE_BUILD=OFF
cmake --build . --config Release

cd ..
mkdir build64_re3
cd build64_re3
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE3=ON -DSINGLE_BUILD=OFF
cmake --build . --config Release

cd ..
mkdir build64_re8
cd build64_re8
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DRE8=ON -DSINGLE_BUILD=OFF
cmake --build . --config Release

cd ..
mkdir build64_dmc5
cd build64_dmc5
cmake .. -G "Visual Studio 16 2019" -A x64 -DDEVELOPER_MODE=ON -DDMC5=ON -DSINGLE_BUILD=OFF
cmake --build . --config Release