#define INITGUID

#include <spdlog/spdlog.h>

#include "DInputHook.hpp"

using namespace std;

static DInputHook* g_dinputHook{ nullptr };

DInputHook::DInputHook(HWND wnd)
    : m_wnd{ wnd },
    m_isIgnoringInput{ false }
{
    if (g_dinputHook == nullptr) {
        if (hook()) {
            spdlog::info("DInputHook hooked successfully.");
            g_dinputHook = this;
        }
        else {
            spdlog::info("DInputHook failed to hook.");
        }
    }
}

DInputHook::~DInputHook() {
    // Explicitly unhook the methods we hooked so we can reset g_dinputHook.
    m_getDeviceStateHook.reset();

    g_dinputHook = nullptr;
}

bool DInputHook::hook() {
    spdlog::info("Entering DInputHook::hook().");

    // All we do here is create an IDirectInputDevice so that we can get the
    // addresses of the methods we want to hook from its vtable.
    using DirectInput8CreateFn = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

    auto dinput8 = LoadLibrary("dinput8.dll");
    auto dinput8Create = (DirectInput8CreateFn)GetProcAddress(dinput8, "DirectInput8Create");

    if (dinput8Create == nullptr) {
        spdlog::info("Failed to find DirectInput8Create.");
        return false;
    }

    spdlog::info("Got DirectInput8Create {:p}", (void*)dinput8Create);

    auto instance = (HINSTANCE)GetModuleHandle(nullptr);
    IDirectInput* dinput{ nullptr };

    if (FAILED(dinput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8W, (LPVOID*)&dinput, nullptr))) {
        spdlog::info("Failed to create IDirectInput.");
        return false;
    }

    spdlog::info("Got IDirectInput {:p}", (void*)dinput);

    IDirectInputDevice* device{ nullptr };

    if (FAILED(dinput->CreateDevice(GUID_SysKeyboard, &device, nullptr))) {
        spdlog::info("Failed to create IDirectInputDevice.");
        dinput->Release();
        return false;
    }

    spdlog::info("Got IDirectInputDevice {:p}", (void*)device);

    // Get the addresses of the methods we want to hook.
    auto getDeviceState = (*(uintptr_t**)device)[9];

    spdlog::info("Got IDirectInputDevice::GetDeviceState {:p}", getDeviceState);

    device->Release();
    dinput->Release();

    // Hook them.
    m_getDeviceStateHook = make_unique<FunctionHook>(getDeviceState, (uintptr_t)&DInputHook::getDeviceState);

    return m_getDeviceStateHook->isValid();
}

HRESULT WINAPI DInputHook::getDeviceState(IDirectInputDevice* device, DWORD size, LPVOID data) {
    auto dinput = g_dinputHook;
    auto originalGetDeviceState= (decltype(DInputHook::getDeviceState)*)dinput->m_getDeviceStateHook->getOriginal();

    spdlog::debug("getDeviceState");

    // If we are ignoring input then we call the original to remove buffered    
    // input events from the devices queue without modifying the out parameters.
    if (dinput->m_isIgnoringInput) {
        device->Unacquire();
        device->SetCooperativeLevel(dinput->m_wnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        device->Acquire();

        if (data == nullptr) {
            return originalGetDeviceState(device, size, data);
        }

        return originalGetDeviceState(device, size, data);
    }

    return originalGetDeviceState(device, size, data);
}
