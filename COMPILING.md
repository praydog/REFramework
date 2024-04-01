# Compiling REFramework

## Necessary prerequisites

A C++23 compatible compiler is required. Visual Studio 2022 is recommended. Compilers other than MSVC have not been tested.

CMake is required.

## Compiling

###  Clone the repository

#### SSH
```
git clone git@github.com:praydog/REFramework.git
```

#### HTTPS
```
git clone https://github.com/praydog/REFramework
```

### Initialize the submodules

```
git submodule update --init --recursive
```

### Set up CMake

#### Command line

```
cmake -S . -B build ./build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release
```

Keep in mind that not supplying a target will build multiple targets, when you may only need one of them.

For example, to build only the RE2 target, you can use the following command:

```
cmake --build ./build --config Release --target RE2
```

#### VSCode

1. Install the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension
2. Open the REFramework folder in VSCode
3. Press `Ctrl+Shift+P` and select `CMake: Configure`
4. When "Select a kit" appears, select `Visual Studio Community 2022 Release - amd64`
5. Select the desired build config (usually `Release` or `RelWithDebInfo`) near the bottom of the window
6. Select the desired build target near the bottom of the window, otherwise all targets will be built
6. You should now be able to compile REFramework by pressing `Ctrl+Shift+P` and selecting `CMake: Build` or by pressing `F7`

#### Batch script

In the root of the repository there is a `build_vs2022.bat` script that will build all targets in Release mode.