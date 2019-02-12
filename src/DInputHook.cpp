#define INITGUID

#include <spdlog/spdlog.h>

#include "REFramework.hpp"
#include "DInputHook.hpp"

using namespace std;

static DInputHook* g_dinputHook{ nullptr };

DInputHook::DInputHook(HWND wnd)
    : m_wnd{ wnd },
    m_isIgnoringInput{ false },
    m_doOnce{ true }
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

    return m_getDeviceStateHook->create();
}

HRESULT DInputHook::getDeviceState_Internal(IDirectInputDevice* device, DWORD size, LPVOID data) {
    auto originalGetDeviceState = (decltype(DInputHook::getDeviceState)*)m_getDeviceStateHook->getOriginal();

    spdlog::debug("getDeviceState");

    // If we are ignoring input then we call the original to remove buffered    
    // input events from the devices queue without modifying the out parameters.
    if (m_isIgnoringInput || m_doOnce) {
        device->Unacquire();
        device->SetCooperativeLevel(m_wnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        device->Acquire();

        m_doOnce = false;
    }

    auto res = originalGetDeviceState(device, size, data);

    // Feed keys back to the framework
    if (res == DI_OK && !m_isIgnoringInput && data != nullptr) {
        g_framework->onDirectInputKeys(*(std::array<uint8_t, 256>*)data);
    }

    return res;
}

HRESULT WINAPI DInputHook::getDeviceState(IDirectInputDevice* device, DWORD size, LPVOID data) {
    return g_dinputHook->getDeviceState_Internal(device, size, data);
}
