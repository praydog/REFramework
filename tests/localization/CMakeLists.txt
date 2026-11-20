cmake_minimum_required(VERSION 3.24)
project(localization_regression LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(REF_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH "REFramework checkout")
set(IMGUI_SOURCE "${REF_SOURCE}/dependencies/imgui")
if(NOT EXISTS "${IMGUI_SOURCE}/imgui.cpp")
    message(FATAL_ERROR "Initialize the ImGui submodule: git submodule update --init --recursive dependencies/imgui")
endif()
add_executable(localization_regression main.cpp
    "${IMGUI_SOURCE}/imgui.cpp"
    "${IMGUI_SOURCE}/imgui_draw.cpp"
    "${IMGUI_SOURCE}/imgui_tables.cpp"
    "${IMGUI_SOURCE}/imgui_widgets.cpp")
target_include_directories(localization_regression PRIVATE "${REF_SOURCE}/src" "${IMGUI_SOURCE}")
target_compile_definitions(localization_regression PRIVATE IMGUI_DISABLE_WIN32_FUNCTIONS IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS)
if(MSVC)
    target_compile_options(localization_regression PRIVATE /utf-8 /bigobj)
endif()
enable_testing()
add_test(NAME localization_regression COMMAND localization_regression)
