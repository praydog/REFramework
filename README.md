# REFramework [![Build status](https://ci.appveyor.com/api/projects/status/65a65id6eletvop4?svg=true)](https://ci.appveyor.com/project/praydog/reframework)
A mod framework, scripting platform, and modding tool for RE Engine games. Inspired by and uses code from [Kanan](https://github.com/cursey/kanan-new)

## Installation
Download the latest files from the [Releases](https://github.com/praydog/REFramework/releases) page.

[AppVeyor Release Builds](https://ci.appveyor.com/project/praydog/reframework/branch/master/artifacts)

[AppVeyor Developer Builds](https://ci.appveyor.com/project/praydog/reframework-dev/build/artifacts)

### Non-VR
* Extract the whole zip file except `openvr_api.dll` into your corresponding game folder.

### VR
* Install SteamVR
* Extract the whole zip file into your corresponding game folder.

[VR Troubleshooting/FAQ](https://github.com/praydog/REFramework/wiki/VR-Troubleshooting)

Example game folder: G:\SteamLibrary\steamapps\common\RESIDENT EVIL 2 BIOHAZARD RE2

Supports both DirectX 11 and DirectX 12.

## Included Mods
* Lua Scripting API (All games)
* VR
  * Generic 6DOF VR support for all games
  * Motion controls for RE2/RE3 (RE7 and RE8 motion controls are still WIP!)
* First Person (RE2, RE3)
* Free Camera (All games)
* Scene Timescale (All games)
* Manual Flashlight (RE2, RE3, RE8)
* FOV Slider & Vignette Disabler (RE2, RE3, RE8)

## Included Fixes
* RE8 Startup Crash
* RE8 Stutters (killing enemies, taking damage, etc...)

## Included Tools (Developer Mode)
* Game Objects Display
* Object Explorer

## Supported Games
* Resident Evil 2
* Resident Evil 3
* Resident Evil 7
* Resident Evil Village
* Devil May Cry 5
* Monster Hunter Rise

## Thanks
[SkacikPL](https://github.com/SkacikPL) for originally creating the Manual Flashlight mod.

[cursey](https://github.com/cursey/) for helping develop the VR component and the scripting system.

[The Hitchhiker](https://github.com/youwereeatenbyalid/) and [alphaZomega](https://github.com/alphazolam) for the great help stress testing, creating scripts for the scripting system, and helpful suggestions.
