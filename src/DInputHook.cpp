#define INITGUID

#include <spdlog/spdlog.h>

#include "REFramework.hpp"
#include "DInputHook.hpp"

using namespace std;

static DInputHook* g_dinput_hook{ nullptr };

DInputHook::DInputHook(HWND wnd)
    : m_wnd{ wnd },
    m_is_ignoring_input{ false },
    m_do_once{ true }
{
    if (g_dinput_hook == nullptr) {
        g_dinput_hook = this;
        if (hook()) {
            spdlog::info("DInputHook hooked successfully.");
        }
        else {
            g_dinput_hook = nullptr;
            spdlog::info("DInputHook failed to hook.");
        }
    }
}

DInputHook::~DInputHook() {
    // Explicitly unhook the methods we hooked so we can reset g_dinputHook.
    m_get_device_state_hook.reset();

    g_dinput_hook = nullptr;
}

bool DInputHook::hook() {
    spdlog::info("Entering DInputHook::hook().");

    // All we do here is create an IDirectInputDevice so that we can get the
    // addresses of the methods we want to hook from its vtable.
    using DirectInput8CreateFn = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

    auto dinput8 = LoadLibrary("dinput8.dll");
    auto dinput8_create = (DirectInput8CreateFn)GetProcAddress(dinput8, "DirectInput8Create");

    if (dinput8_create == nullptr) {
        spdlog::info("Failed to find DirectInput8Create.");
        return false;
    }

    spdlog::info("Got DirectInput8Create {:p}", (void*)dinput8_create);

    auto instance = (HINSTANCE)GetModuleHandle(nullptr);
    IDirectInput* dinput{ nullptr };

    if (FAILED(dinput8_create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8W, (LPVOID*)&dinput, nullptr))) {
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
    auto get_device_state = (*(uintptr_t**)device)[9];

    spdlog::info("Got IDirectInputDevice::GetDeviceState {:x}", get_device_state);

    device->Release();
    dinput->Release();

    // Hook them.
    m_get_device_state_hook = make_unique<FunctionHook>(get_device_state, (uintptr_t)&DInputHook::get_device_state);

    return m_get_device_state_hook->create();
}

HRESULT DInputHook::get_device_state_internal(IDirectInputDevice* device, DWORD size, LPVOID data) {
    auto original_get_device_state = (decltype(DInputHook::get_device_state)*)m_get_device_state_hook->get_original();

    spdlog::debug("getDeviceState");

    // If we are ignoring input then we call the original to remove buffered    
    // input events from the devices queue without modifying the out parameters.
    if ((m_is_ignoring_input || m_do_once) && size == 256) {
        device->Unacquire();
        device->SetCooperativeLevel(m_wnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        device->Acquire();

        m_do_once = false;
    }

    auto res = original_get_device_state(device, size, data);

    // Feed keys back to the framework
    if (res == DI_OK && !m_is_ignoring_input && data != nullptr && size == 256) {
        g_framework->on_direct_input_keys(*(std::array<uint8_t, 256>*)data);
    }

    return res;
}

HRESULT WINAPI DInputHook::get_device_state(IDirectInputDevice* device, DWORD size, LPVOID data) {
    return g_dinput_hook->get_device_state_internal(device, size, data);
}
