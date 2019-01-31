set(HOOKS_SRC
    DInputHook.hpp
    DInputHook.cpp
    D3D11Hook.hpp
    D3D11Hook.cpp
    WindowsMessageHook.hpp
    WindowsMessageHook.cpp

    ComponentHook.hpp
    ComponentHook.cpp
    PositionHooks.hpp
    PositionHooks.cpp
)

set(MODS_SRC
    ComponentTraverser.hpp
    ComponentTraverser.cpp
    FirstPerson.hpp
    FirstPerson.cpp
)

set(SDK_SRC
    ReClass.hpp
    ReClass_Internal.hpp
    REString.hpp
    RETransform.hpp
)

set(RE2IMGUI_SRC
    re2-imgui/imgui_impl_win32.h
    re2-imgui/imgui_impl_win32.cpp
    re2-imgui/imgui_impl_dx11.h
    re2-imgui/imgui_impl_dx11.cpp
)

source_group("re2-imgui", FILES ${RE2IMGUI_SRC})
source_group("Hooks" FILES ${HOOKS_SRC})
source_group("Mods" FILES ${MODS_SRC})
source_group("SDK" FILES ${SDK_SRC})

add_library(RE2 SHARED
            FunctionHook.hpp
            FunctionHook.cpp
            Main.cpp

            Mod.hpp
            Mods.hpp
            Mods.cpp
            REFramework.hpp
            REFramework.cpp

            Address.hpp
            Address.cpp

            ${SDK_SRC}
            ${MODS_SRC}
            ${HOOKS_SRC}
            ${RE2IMGUI_SRC}
)

target_link_libraries(RE2 PUBLIC imgui minhook d3d11)
target_compile_options(RE2 PUBLIC /GS- /GR- /MP)
target_compile_features(RE2 PUBLIC cxx_std_17)

set_target_properties(RE2 PROPERTIES OUTPUT_NAME dinput8)
