#include <thread>
#include <chrono>
#include <windows.h>

#include "REFramework.hpp"

HMODULE g_dinput = 0;

extern "C" {
    __declspec(dllexport) HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, const IID& riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
        return ((decltype(DirectInput8Create)*)GetProcAddress(g_dinput, "DirectInput8Create"))(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }
}

void startupThread() {
#ifndef NDEBUG
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    char buffer[MAX_PATH]{ 0 };
    GetSystemDirectory(buffer, MAX_PATH);

    g_dinput = LoadLibrary((std::string{ buffer } +"\\dinput8.dll").c_str());

    g_framework = std::make_unique<REFramework>();
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startupThread, nullptr, 0, nullptr);
    }

    return TRUE;
}
