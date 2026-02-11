#pragma once

#include "Mod.hpp"
#include "sdk/ResourceManager.hpp"

#include <safetyhook.hpp>
#include <deque>
#include <map>
#include <regex>
#include <shared_mutex>
#include <thread>
#include <unordered_set>
#include <spdlog/spdlog.h>

class FaultyFileDetector : public Mod {
public:
    FaultyFileDetector();

    std::string_view get_name() const override { return "FaultyFileDetector"; };

    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;
    void on_draw_ui() override;

private:
    static sdk::Resource* create_resource_hook_wrapper(sdk::ResourceManager *resource_manager, void* type_info, wchar_t* name);
    sdk::Resource* create_resource_hook(sdk::ResourceManager *resource_manager, void* type_info, wchar_t* name);

    static void resource_parse_finish_hook_wrapper(safetyhook::Context& ctx);
    void resource_parse_finish_hook(safetyhook::Context& ctx);

    static void resource_set_argument_hook_wrapper(safetyhook::Context& ctx);
    void resource_set_argument_hook(safetyhook::Context& ctx);

    static void on_pak_load_result(bool success, std::wstring_view pak_name);

    void try_add_to_faulty_list(std::wstring_view filename);

    static void listen_to_pak_load_result_from_integrity_check_bypass();
    static void cache_patch_version();

    bool scan_resource_process_parse_and_hook();

    std::optional<std::string> m_blocking_error{};

    std::unordered_set<std::wstring> m_faulty_files{}; // All faulty files for spam detection
    std::deque<std::wstring> m_recent_faulty_files{}; // Recent files for display
    std::shared_mutex m_mutex{};
    std::shared_ptr<spdlog::logger> m_logger{};
    safetyhook::InlineHook m_create_resource_original{};
    safetyhook::MidHook m_resource_parse_finish_hook{};
    std::map<std::thread::id, void*> m_resource_by_thread_map{};
    std::vector<safetyhook::MidHook> m_resource_parse_finish_hooks{};
    std::vector<safetyhook::MidHook> m_resource_set_argument_hooks{};

    ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), false) };
    ModInt32::Ptr m_max_recent_files{ ModInt32::create(generate_name("MaxRecentFiles"), 100) };

    static inline FaultyFileDetector* s_instance{nullptr};

    ValueList m_options{
        *m_enabled,
        *m_max_recent_files,
    };
};