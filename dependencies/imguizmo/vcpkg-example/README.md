# ImGuizmo example

ImGuizmo example app that uses [cmake][cmake] and [vcpkg][vcpkg]

## Setup

Assuming cmake and vcpkg are installed, all that is required is to add the vcpkg toolchain file to
the cmake configure and all dependencies will be downloaded and installed into the project

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystem/vcpkg.cmake
```

### Visual Studio

Rather than running directly, IDEs usually configure the cmake. Visual studio can be integrated
directly with the `vcpkg integrate install` command or by specifying the path to the toolchain file
in the project cmake settings

### VSCode

When using the cmake tools extension the command can be added to the `.vscode/settings.json` file as
follows

```json
{
    "cmake.configureArgs": [
        "-DCMAKE_TOOLCHAIN_FILE=C:\\dev\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake"
    ]
}
```

> :heavy_check_mark: If you include the settings file in your repo, a good practice would be to use
an environment variable for the path to the vcpkg install and variable substitution in the toolchain
file path so the settings work across machines. eg `"-DCMAKE_TOOLCHAIN_FILE={$env:VCPKG_ROOT}\\scripts\\buildsystems\\vcpkg.cmake"`

This can also be added to the system-wide settings rather than added to each project individually

## How It Works

The example contains a `vcpkg.json` manifest file that contains the dependencices. When cmake is
supplied with the vcpkg toolchain file, the script detects this manifest file and then downloads and
compiles the dependencies there-in

In `CMakeLists.txt` the `find_package(imguizmo CONFIG REQUIRED)` command finds the cmake config
added by the vcpkg install and `target_link_libraries(example-app PRIVATE imguizmo::imguizmo)` adds
the includes and lib to the target

[cmake]: https://cmake.org/
[vcpkg]: https://github.com/microsoft/vcpkg