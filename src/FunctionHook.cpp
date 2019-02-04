#include <spdlog/spdlog.h>
#include <MinHook.h>

#include "FunctionHook.hpp"

using namespace std;


bool g_isMinHookInitialized{ false };

FunctionHook::FunctionHook(Address target, Address destination)
    : m_target{ 0 },
    m_destination{ 0 },
    m_original{ 0 }
{
    spdlog::info("Attempting to hook {:p}->{:p}", target.ptr(), destination.ptr());

    // Initialize MinHook if it hasn't been already.
    if (!g_isMinHookInitialized && MH_Initialize() == MH_OK) {
        g_isMinHookInitialized = true;
    }

    // Create and enable the hook, only setting our member variables if it was
    // successful.
    uintptr_t original{ 0 };

    if (MH_CreateHook(target.as<LPVOID>(), destination.as<LPVOID>(), (LPVOID*)&original) == MH_OK &&
        MH_EnableHook(target.as<LPVOID>()) == MH_OK)
    {
        m_target = target;
        m_destination = destination;
        m_original = original;
        spdlog::info("Hooked {:p}->{:p}", target.ptr(), destination.ptr());
    }
    else {
        spdlog::error("Failed to hook {:p}", target.ptr());
    }
}

FunctionHook::~FunctionHook() {
    remove();
}

bool FunctionHook::remove() {
    // Don't try to remove invalid hooks.
    if (m_original == 0) {
        return true;
    }

    // Disable then remove the hook.
    if (MH_DisableHook((LPVOID)m_target) != MH_OK ||
        MH_RemoveHook((LPVOID)m_target) != MH_OK) {
        return false;
    }

    // Invalidate the members.
    m_target = 0;
    m_destination = 0;
    m_original = 0;

    return true;
}