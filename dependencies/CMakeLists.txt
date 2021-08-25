add_subdirectory(spdlog)

SET(GLM_TEST_ENABLE OFF CACHE BOOL "Build unit tests" FORCE)
SET(BUILD_STATIC_LIBS ON)
add_subdirectory(glm)

project(minhook)

add_library(minhook STATIC
            minhook/src/buffer.c
            minhook/src/buffer.h
            minhook/src/hook.c
            minhook/src/trampoline.c
            minhook/src/trampoline.h
            minhook/src/hde/hde32.c
            minhook/src/hde/hde32.h
            minhook/src/hde/hde64.c
            minhook/src/hde/hde64.h
            minhook/src/hde/pstdint.h
            minhook/src/hde/table32.h
            minhook/src/hde/table64.h)

project(imgui)

add_library(imgui STATIC
            imgui/imgui_demo.cpp
            imgui/imgui_widgets.cpp
            imgui/imgui_draw.cpp
            imgui/imgui_tables.cpp
            imgui/imgui.cpp)

add_library(openvr INTERFACE openvr/headers/openvr.h)
file(COPY openvr/lib/win64/openvr_api.lib DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
file(COPY openvr/bin/win64/openvr_api.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_compile_definitions(imgui PUBLIC IMGUI_USER_CONFIG="../src/re2-imgui/re2_imconfig.hpp")