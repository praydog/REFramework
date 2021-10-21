project(${GAME_NAME})

set(HOOKS_SRC
    DInputHook.hpp
    DInputHook.cpp
    D3D11Hook.hpp
    D3D11Hook.cpp
    D3D12Hook.hpp
    D3D12Hook.cpp
    WindowsMessageHook.hpp
    WindowsMessageHook.cpp

    mods/Hooks.hpp
    mods/Hooks.cpp
)

set(MODS_SRC
    mods/Camera.hpp
    mods/Camera.cpp
    mods/DeveloperTools.hpp
    mods/DeveloperTools.cpp
    mods/FirstPerson.hpp
    mods/FirstPerson.cpp
    mods/FreeCam.hpp
    mods/FreeCam.cpp
    mods/IntegrityCheckBypass.hpp
    mods/IntegrityCheckBypass.cpp
    mods/ManualFlashlight.hpp
    mods/ManualFlashlight.cpp
    mods/ObjectExplorer.hpp
    mods/ObjectExplorer.cpp
    mods/Scene.hpp
    mods/Scene.cpp
    mods/VR.hpp
    mods/VR.cpp
    mods/vr/Bindings.cpp
    mods/vr/D3D11Component.hpp
    mods/vr/D3D11Component.cpp
    mods/vr/D3D12Component.hpp
    mods/vr/D3D12Component.cpp
    mods/ScriptRunner.hpp
    mods/ScriptRunner.cpp
)

set(SDK_SRC
    sdk/ReClass.hpp
    sdk/ReClass_Internal.hpp
    sdk/ReClass_Internal_RE3.hpp
    sdk/ReClass_Internal_RE7.hpp
    sdk/ReClass_Internal_RE8.hpp
    sdk/ReClass_Internal_DMC5.hpp
    sdk/Enums_Internal.hpp

    sdk/Application.hpp
    sdk/Application.cpp
    sdk/REArray.hpp
    sdk/REComponent.hpp
    sdk/REContext.hpp
    sdk/REContext.cpp
    sdk/REGlobals.hpp
    sdk/REGlobals.cpp
    sdk/REManagedObject.hpp
    sdk/REMath.hpp
    sdk/RENativeArray.hpp
    sdk/REString.hpp
    sdk/RETransform.hpp
    sdk/REType.hpp
    sdk/REType.cpp
    sdk/RETypeCLR.hpp
    sdk/RETypes.hpp
    sdk/RETypes.cpp
    sdk/RETypeDB.hpp
    sdk/RETypeDB.cpp
    sdk/RETypeDefinition.hpp
    sdk/RETypeDefinition.cpp
    sdk/REVariableDescriptor.hpp
    sdk/Renderer.hpp
    sdk/Renderer.cpp
    sdk/RopewaySweetLightManager.hpp
    sdk/SceneManager.hpp
    sdk/SceneManager.cpp
)

set(RE2IMGUI_SRC
    re2-imgui/imgui_impl_win32.h
    re2-imgui/imgui_impl_win32.cpp
    re2-imgui/imgui_impl_dx11.h
    re2-imgui/imgui_impl_dx11.cpp
    re2-imgui/imgui_impl_dx12.h
    re2-imgui/imgui_impl_dx12.cpp
    re2-imgui/re2_imconfig.hpp
)

set(UTILITY_SRC
    utility/Address.hpp
    utility/Address.cpp
    utility/Config.hpp
    utility/Config.cpp
    utility/FunctionHook.hpp
    utility/FunctionHook.cpp
    utility/Memory.hpp
    utility/Memory.cpp
    utility/Module.hpp
    utility/Module.cpp
    utility/Patch.hpp
    utility/Patch.cpp
    utility/Pattern.hpp
    utility/Pattern.cpp
    utility/Scan.hpp
    utility/Scan.cpp
    utility/String.hpp
    utility/String.cpp
)

set(FRAMEWORK_SRC
    Mod.hpp
    Mods.hpp
    Mods.cpp
    REFramework.hpp
    REFramework.cpp
)

source_group("re2-imgui" FILES ${RE2IMGUI_SRC})
source_group("Hooks" FILES ${HOOKS_SRC})
source_group("Mods" FILES ${MODS_SRC})
source_group("SDK" FILES ${SDK_SRC})
source_group("Utility" FILES ${UTILITY_SRC})
source_group("Framework" FILES ${FRAMEWORK_SRC})

add_library(${GAME_NAME} SHARED
            Main.cpp
            LicenseStrings.hpp
            ${SDK_SRC}
            ${MODS_SRC}
            ${HOOKS_SRC}
            ${RE2IMGUI_SRC}
            ${UTILITY_SRC}
            ${FRAMEWORK_SRC}
)

target_link_libraries(${GAME_NAME} PUBLIC lua sol2 imgui minhook d3d11 glm_static shlwapi ${CMAKE_SOURCE_DIR}/dependencies/openvr/lib/win64/openvr_api.lib)
target_compile_options(${GAME_NAME} PUBLIC /GS-)
target_compile_features(${GAME_NAME} PUBLIC cxx_std_20)

include_directories(${GAME_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${GAME_NAME} PROPERTIES OUTPUT_NAME dinput8)

target_compile_definitions(${GAME_NAME} PUBLIC IMGUI_USER_CONFIG="re2-imgui/re2_imconfig.hpp")

if(DEVELOPER_MODE)
    target_compile_definitions(${GAME_NAME} PUBLIC DEVELOPER)
    target_compile_options(${GAME_NAME} PUBLIC /EHa)
endif()

if (RE2)
    target_compile_definitions(${GAME_NAME} PUBLIC RE2)
endif()

if (RE3)
    target_compile_definitions(${GAME_NAME} PUBLIC RE3)
endif()

if (RE7)
    target_compile_definitions(${GAME_NAME} PUBLIC RE7)
endif()

if (RE8)
    target_compile_definitions(${GAME_NAME} PUBLIC RE8)
endif()

if (DMC5)
    target_compile_definitions(${GAME_NAME} PUBLIC DMC5)
endif()

if (MHRISE)
    target_compile_definitions(${GAME_NAME} PUBLIC MHRISE)
endif()


if (BAREBONES)
    target_compile_definitions(${GAME_NAME} PUBLIC BAREBONES)
endif()
