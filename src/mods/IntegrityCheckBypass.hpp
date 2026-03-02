#pragma once

#include <memory>
#include <string_view>
#include <regex>

#include "Mod.hpp"
#include "utility/Patch.hpp"
#include "utility/FunctionHook.hpp"
#include "utility/FunctionHookMinHook.hpp"

// Always on for RE3
// Because we use hooks that modify the integrity of the executable
// And RE3 has unfortunately decided to implement an integrity check on the executable code of the process
class IntegrityCheckBypass : public Mod {
public:
    std::string_view get_name() const override { return "IntegrityCheckBypass"; };
    std::optional<std::string> on_initialize() override;

    void on_frame() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;
    void on_draw_ui() override;
    
    static void ignore_application_entries();
    static void immediate_patch_re8();
    static void immediate_patch_re4();
    static void immediate_patch_dd2();
    static void immediate_patch_re9();
    static void remove_stack_destroyer();

    static void setup_pristine_syscall();
    static void fix_virtual_protect();

    static void hook_add_vectored_exception_handler();
    static void hook_rtl_exit_user_process();

    static void allow_veh() {
        s_veh_allowed = true;
    }
    
    static bool is_veh_called() {
        return s_veh_called;
    }

    static std::shared_ptr<IntegrityCheckBypass>& get_shared_instance();

private:
    static void* renderer_create_blas_hook(void* a1, void* a2, void* a3, void* a4, void* a5);
    static inline std::unique_ptr<FunctionHook> s_renderer_create_blas_hook{};
    static inline uint32_t* s_corruption_when_zero{ nullptr };
    static inline uint32_t s_last_non_zero_corruption{ 8 }; // What I've seen it default to

    static void sha3_rsa_code_midhook(safetyhook::Context& context);
    static void pak_load_check_function(safetyhook::Context& context);
    static void patch_version_hook(safetyhook::Context& context);
    static int scan_patch_files_count();
    static void restore_unencrypted_paks();
    static inline safetyhook::MidHook s_sha3_rsa_code_midhook;
    static inline safetyhook::MidHook s_pak_load_check_function_hook;
    static inline safetyhook::MidHook s_patch_version_hook;
    static inline std::optional<uintptr_t> s_sha3_code_end{};
    static inline int32_t s_sha3_reg_index{-1};
    static inline int32_t s_patch_version_reg_index{-1};
    static inline int s_patch_count;
    static inline bool s_patch_count_checked;

    static void anti_debug_watcher();
    static void init_anti_debug_watcher();
    static void nuke_heap_allocated_code(uintptr_t addr);
    static inline std::unique_ptr<std::jthread> s_anti_anti_debug_thread{nullptr};

    static BOOL WINAPI virtual_protect_impl(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
    static BOOL WINAPI virtual_protect_hook(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
    
    static PVOID WINAPI add_vectored_exception_handler_hook(ULONG FirstHandler, PVECTORED_EXCEPTION_HANDLER VectoredHandler);
    static inline bool s_veh_allowed{false};
    static inline bool s_veh_called{false};
    using NtProtectVirtualMemory_t =  NTSTATUS(NTAPI*)(HANDLE ProcessHandle, PVOID* BaseAddress, SIZE_T* NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
    static inline NtProtectVirtualMemory_t s_pristine_protect_virtual_memory{ nullptr };
    static inline NtProtectVirtualMemory_t s_og_protect_virtual_memory{ nullptr };;

    // Using minhook because safetyhook crashes on trying to hook VirtualProtect
    static inline std::unique_ptr<FunctionHookMinHook> s_virtual_protect_hook{};
    static inline std::unique_ptr<FunctionHookMinHook> s_add_vectored_exception_handler_hook{};

    static void* rtl_exit_user_process_hook(uint32_t code);
    static inline std::unique_ptr<FunctionHookMinHook> s_rtl_exit_user_process_hook{};
#ifdef RE3
    // This is what the game uses to bypass its integrity checks altogether or something
    bool* m_bypass_integrity_checks{ nullptr };
#else
    void disable_update_timers(std::string_view name) const;

    std::vector<std::unique_ptr<Patch>> m_patches{};
#endif

#pragma region Custom PAK directory loading
    constexpr static const char* CUSTOM_PAK_DIRECTORY_PATH = "pak_mods";
    constexpr static const char* PAK_EXTENSION_NAME = ".pak";
    constexpr static const wchar_t* PAK_EXTENSION_NAME_W = L".pak";
    constexpr static const wchar_t* SUB_PATCH_SCAN_REGEX = LR"(re_chunk_000\.pak\.sub_000\.pak\.patch_(\d+)\.pak)";

    static void find_try_hook_via_file_load_win32_create_file(uintptr_t pak_load_func_addr);
    static void via_file_prepare_to_create_file_w_hook_wrappper(safetyhook::Context& context);
    static void directstorage_open_pak_hook_wrappper(safetyhook::Context& context);

    int cache_and_count_custom_pak_in_directory();
    std::optional<int> extract_patch_num_from_path(std::wstring &path);
    void via_file_prepare_to_create_file_w_hook(safetyhook::Context& context);
    void directstorage_open_pak_hook(safetyhook::Context& context);
    void correct_pak_load_path(safetyhook::Context& context, int register_index);

    const ModToggle::Ptr m_load_pak_directory{ ModToggle::create(generate_name("LoadPakDirectory"), true) };

    static inline std::vector<safetyhook::MidHook> s_before_create_file_w_hooks{};
    static inline safetyhook::MidHook s_directstorage_open_pak_hook{};
    static inline int s_base_directory_patch_count{0};

    std::vector<std::wstring> m_custom_pak_in_directory_paths{};
    bool m_custom_pak_in_directory_paths_cached{ false };
    std::wregex m_sub_patch_scan_regex{SUB_PATCH_SCAN_REGEX, std::regex::ECMAScript};

    ValueList m_options{
        *m_load_pak_directory
    };
#pragma endregion 
};