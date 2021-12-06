#include <thread>
#include <chrono>
#include <windows.h>

#include "../include/API.hpp"

lua_State* g_lua = nullptr;

void on_lua_state_created(lua_State* l) {
    g_lua = l;

    MessageBox(NULL, "Lua State Created", "DLL", MB_OK);
}

void on_lua_state_destroyed(lua_State* l) {
    g_lua = nullptr;

    MessageBox(NULL, "Lua State Destroyed", "DLL", MB_OK);
}

void on_frame() {
    OutputDebugString("Example Frame");
}

void startup_thread() {
    MessageBox(NULL, "Hello from the DLL!", "DLL", MB_OK);

    reframework_on_lua_state_created(on_lua_state_created);
    reframework_on_lua_state_destroyed(on_lua_state_destroyed);
    reframework_on_frame(on_frame);
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, nullptr, 0, nullptr);
    }

    return TRUE;
}