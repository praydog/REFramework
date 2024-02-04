#pragma once

#include <memory>
#include <string_view>

#include "Mod.hpp"
#include "utility/Patch.hpp"
#include "utility/FunctionHook.hpp"

// Always on for RE3
// Because we use hooks that modify the integrity of the executable
// And RE3 has unfortunately decided to implement an integrity check on the executable code of the process
class IntegrityCheckBypass : public Mod {
public:
    std::string_view get_name() const override { return "IntegrityCheckBypass"; };
    std::optional<std::string> on_initialize() override;

    void on_frame() override;
    
    static void ignore_application_entries();
    static void immediate_patch_re8();
    static void immediate_patch_re4();
    static void remove_stack_destroyer();

    static void setup_pristine_syscall();
    static void fix_virtual_protect();

private:
    static BOOL WINAPI virtual_protect_impl(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
    static BOOL WINAPI virtual_protect_hook(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
    
    using NtProtectVirtualMemory_t =  NTSTATUS(NTAPI*)(HANDLE ProcessHandle, PVOID* BaseAddress, SIZE_T* NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
    static inline NtProtectVirtualMemory_t s_pristine_protect_virtual_memory{ nullptr };
    static inline NtProtectVirtualMemory_t s_og_protect_virtual_memory{ nullptr };

    static inline std::unique_ptr<FunctionHook> s_get_proc_address_hook{};
    static inline std::unique_ptr<FunctionHook> s_virtual_protect_hook{};

#ifdef RE3
    // This is what the game uses to bypass its integrity checks altogether or something
    bool* m_bypass_integrity_checks{ nullptr };
#else
    void disable_update_timers(std::string_view name) const;

    std::vector<std::unique_ptr<Patch>> m_patches{};
#endif
};