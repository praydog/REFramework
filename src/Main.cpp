#include <chrono>
#include <mutex>
#include <thread>
#include <windows.h>
#include <winternl.h>

#include <utility/Module.hpp>
#include <utility/Thread.hpp>

#include <safetyhook/allocator.hpp>

// minhook, used for AllocateBuffer
extern "C" {
#include <../buffer.h>
};

#include "mods/IntegrityCheckBypass.hpp"
#include "ExceptionHandler.hpp"
#include "REFramework.hpp"

HMODULE g_dinput = 0;
std::mutex g_load_mutex{};
extern bool g_success_made_ldr_notification;

void failed() {
    MessageBox(0, "REFramework: Unable to load the original dinput8.dll. Please report this to the developer.", "REFramework", 0);
    ExitProcess(0);
}

bool load_dinput8() {
    std::scoped_lock _{g_load_mutex};

    if (g_dinput) {
        return true;
    }

    wchar_t buffer[MAX_PATH]{0};
    if (GetSystemDirectoryW(buffer, MAX_PATH) != 0) {
        // Load the original dinput8.dll
        if ((g_dinput = LoadLibraryW((std::wstring{buffer} + L"\\dinput8.dll").c_str())) == NULL) {
            failed();
            return false;
        }

        return true;
    }

    failed();
    return false;
}

extern "C" {
// DirectInput8Create wrapper for dinput8.dll
__declspec(dllexport) HRESULT WINAPI
    direct_input8_create(HINSTANCE hinst, DWORD dw_version, const IID& riidltf, LPVOID* ppv_out, LPUNKNOWN punk_outer) {
// This needs to be done because when we include dinput.h in DInputHook,
// It is a redefinition, so we assign an export by not using the original name
#pragma comment(linker, "/EXPORT:DirectInput8Create=direct_input8_create")

    load_dinput8();
    return ((decltype(DirectInput8Create)*)GetProcAddress(g_dinput, "DirectInput8Create"))(hinst, dw_version, riidltf, ppv_out, punk_outer);
}
}

void startup_thread(HMODULE reframework_module) {
    // We will set it once here, then do it continuously
    // every now and then because it gets replaced
    reframework::setup_exception_handler();

#ifndef NDEBUG
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    if (load_dinput8()) {
        g_framework = std::make_unique<REFramework>(reframework_module);

        const auto our_dll = utility::get_module_within(&load_dinput8);

#if defined(MHRISE)
        if (our_dll) {
            if (!g_success_made_ldr_notification) {
                utility::spoof_module_paths_in_exe_dir();
            }
            utility::unlink(*our_dll);
        }
#elif defined (DD2) || TDB_VER >= 74
        if (!g_success_made_ldr_notification) {
            utility::spoof_module_paths_in_exe_dir();
        }
#endif
    }
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        REFramework::set_reframework_module((HMODULE)handle);

        const auto game = utility::get_executable();
        const auto module_size = utility::get_module_size(game).value_or(0);
        const auto halfway_module = (uintptr_t)game + (module_size / 2);

        // Need to pin the safetyhook allocator because it destroys itself
        static auto sh_allocator = safetyhook::Allocator::global();
        // 1MB
        intptr_t requested_size = 1 * 1024 * 1024;
        // Keep attempting to allocate and reduce requested size by 1 page until successful
        // 1MB is just the higher end of the range, it will allocate less if it can
        // even 10kb would be enough for a decent amount of hooks, but we want as much as we can near the middle
        while (requested_size > 0 && !sh_allocator->allocate_near({(uint8_t*)halfway_module}, requested_size)) {
            requested_size -= 0x1000; // Size of page
        }

        AllocateBuffer((LPVOID)halfway_module); // minhook

        IntegrityCheckBypass::setup_pristine_syscall();
        IntegrityCheckBypass::hook_add_vectored_exception_handler();

        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, handle, 0, nullptr);
    }

    return TRUE;
}
