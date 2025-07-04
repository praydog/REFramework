# Building REFramework for Selected Games Only

This guide explains how to build REFramework for only MHRISE, DD2, and MHWILDS.

## Prerequisites

- Visual Studio 2022 with C++ development tools
- CMake (installed with Visual Studio or separately)
- Git

## Quick Build

Simply run the provided batch file:
```batch
build_selected_games.bat
```

## Manual Build Steps

1. **Open a Developer Command Prompt for VS 2022** or regular Command Prompt

2. **Navigate to the REFramework directory**
   ```batch
   cd C:\path\to\REFramework
   ```

3. **Update submodules**
   ```batch
   git submodule update --init --recursive
   ```

4. **Create and enter build directory**
   ```batch
   mkdir build64_selected
   cd build64_selected
   ```

5. **Configure CMake**
   ```batch
   cmake .. -G "Visual Studio 17 2022" -A x64 -DDEVELOPER_MODE=ON
   ```

6. **Build only the selected games**
   ```batch
   cmake --build . --config Release --target MHRISE
   cmake --build . --config Release --target DD2
   cmake --build . --config Release --target MHWILDS
   ```

## Output

The compiled DLLs will be located in:
- `build64_selected\bin\MHRISE\dinput8.dll` - For Monster Hunter Rise
- `build64_selected\bin\DD2\dinput8.dll` - For Dragon's Dogma 2
- `build64_selected\bin\MHWILDS\dinput8.dll` - For Monster Hunter Wilds

## Notes

- This method only builds the specific game targets, saving compile time
- Each DLL is optimized for its respective game
- The build process skips other games like RE2, RE3, RE4, etc.
- If you need SDK libraries for plugin development, add the corresponding SDK flags:
  ```batch
  cmake .. -G "Visual Studio 17 2022" -A x64 -DDEVELOPER_MODE=ON -DREF_BUILD_MHRISE_SDK=ON -DREF_BUILD_DD2_SDK=ON -DREF_BUILD_MHWILDS_SDK=ON
  ```