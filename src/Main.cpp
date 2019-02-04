#include <thread>
#include <chrono>
#include <windows.h>

#include "REFramework.hpp"

HMODULE g_dinput = 0;

extern "C" {
    // DirectInput8Create wrapper for dinput8.dll
    __declspec(dllexport) HRESULT WINAPI DirectInput8Create_(HINSTANCE hinst, DWORD dwVersion, const IID& riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
        // This needs to be done because when we include dinput.h in DInputHook,
        // It is a redefinition, so we assign an export by not using the original name
        #pragma comment(linker, "/EXPORT:DirectInput8Create=DirectInput8Create_")
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

    wchar_t buffer[MAX_PATH]{ 0 };
    if (GetSystemDirectoryW(buffer, MAX_PATH) != 0) {
        // Load the original dinput8.dll
        g_dinput = LoadLibraryW((std::wstring{ buffer } + L"\\dinput8.dll").c_str());
        g_framework = std::make_unique<REFramework>();
    }
    else {
        MessageBox(0, "REFramework: Unable to load the original dinput8.dll. Please report this to the developer.", "REFramework", 0);
        ExitProcess(0);
    }
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startupThread, nullptr, 0, nullptr);
    }

    return TRUE;
}
