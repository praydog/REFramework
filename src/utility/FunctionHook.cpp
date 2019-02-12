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

    // Create the hook. Call create afterwards to prevent race conditions accessing FunctionHook before it leaves its constructor.
    if (MH_CreateHook(target.as<LPVOID>(), destination.as<LPVOID>(), (LPVOID*)&m_original) == MH_OK) {
        m_target = target;
        m_destination = destination;

        spdlog::info("Hook init successful {:p}->{:p}", target.ptr(), destination.ptr());
    }
    else {
        spdlog::error("Failed to hook {:p}", target.ptr());
    }
}

FunctionHook::~FunctionHook() {
    remove();
}

bool FunctionHook::create() {
    if (m_target == 0 || m_destination == 0 || m_original == 0) {
        spdlog::error("FunctionHook not initialized");
        return false;
    }

    if (MH_EnableHook((LPVOID)m_target) != MH_OK) {
        m_original = 0;
        m_destination = 0;
        m_target = 0;

        spdlog::error("Failed to hook {:x}", m_target);
        return false;
    }

    spdlog::info("Hooked {:x}->{:x}", m_target, m_destination);
    return true;
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