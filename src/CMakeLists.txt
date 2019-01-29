add_library(RE2 SHARED
            FunctionHook.hpp
            FunctionHook.cpp
            Main.cpp

            D3D11Hook.hpp
            D3D11Hook.cpp
            WindowsMessageHook.hpp
            WindowsMessageHook.cpp

            ReClass.hpp
            ReClass_Internal.hpp
            REFramework.hpp
            REFramework.cpp

            Address.hpp
            Address.cpp

            ${CMAKE_SOURCE_DIR}/dependencies/imgui/examples/imgui_impl_win32.h
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/examples/imgui_impl_win32.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/examples/imgui_impl_dx11.h
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/examples/imgui_impl_dx11.cpp)

target_link_libraries(RE2 PUBLIC imgui minhook d3d11)
target_compile_options(RE2 PUBLIC /GS- /GR- /MP)
target_compile_features(RE2 PUBLIC cxx_std_17)

set_target_properties(RE2 PROPERTIES OUTPUT_NAME dinput8)
