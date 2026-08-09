git pull --recurse-submodules
git submodule update --init --recursive
mkdir build64_vs2026
cd build64_vs2026
cmake .. -G "Visual Studio 18 2026" -A x64 -DDEVELOPER_MODE=ON
cmake --build . --config Release
