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

add_library(lua STATIC
            lua/src/lapi.c
            lua/src/lauxlib.c
            lua/src/lbaselib.c
            lua/src/lcode.c
            lua/src/lcode.h
            lua/src/lcorolib.c
            lua/src/lctype.c
            lua/src/lctype.h
            lua/src/ldblib.c
            lua/src/ldebug.c
            lua/src/ldo.c
            lua/src/ldump.c
            lua/src/lfunc.c
            lua/src/lfunc.h
            lua/src/lgc.c
            lua/src/lgc.h
            lua/src/linit.c
            lua/src/liolib.c
            lua/src/llex.c
            lua/src/lmathlib.c
            lua/src/lmem.c
            lua/src/loadlib.c
            lua/src/lobject.c
            lua/src/lobject.h
            lua/src/lopcodes.c
            lua/src/lopcodes.h
            lua/src/loslib.c
            lua/src/lparser.c
            lua/src/lstate.c
            lua/src/lstring.c
            lua/src/lstrlib.c
            lua/src/ltable.c
            lua/src/ltablib.c
            lua/src/ltm.c
            #lua/src/lua.c
            #lua/src/luac.c
            lua/src/luaconf.h
            lua/src/lundump.c
            lua/src/lutf8lib.c
            lua/src/lvm.c
            lua/src/lzio.c)
target_include_directories(lua PUBLIC lua/src/)
target_compile_options(lua PUBLIC /MT)

add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE sol2/single/include)

