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
#include <utility/Scan.hpp>

class FaultyFileDetector : public Mod {
public:
    enum FaultyTier {
        None = 0,
        Warning = 1,
        Severe = 2,
    };

    FaultyFileDetector();

    std::string_view get_name() const override { return "FaultyFileDetector"; };

    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;
    void on_draw_ui() override;

    static void early_init();

private:
    static sdk::Resource* create_resource_hook_wrapper(sdk::ResourceManager *resource_manager, void* type_info, wchar_t* name);
    sdk::Resource* create_resource_hook(sdk::ResourceManager *resource_manager, void* type_info, wchar_t* name);

    static void resource_parse_finish_hook_wrapper(safetyhook::Context& ctx);
    void resource_parse_finish_hook(safetyhook::Context& ctx);

    static void resource_parse_open_stream_failed_hook_wrapper(safetyhook::Context& ctx);
    void resource_parse_open_stream_failed_hook(safetyhook::Context& ctx);

    static void resource_set_argument_hook_wrapper(safetyhook::Context& ctx);
    void resource_set_argument_hook(safetyhook::Context& ctx);

    static void on_pak_load_result(bool success, std::wstring_view pak_name);
    static void cache_patch_version();

    void try_add_to_faulty_list(std::wstring_view filename, FaultyTier tier = FaultyTier::Severe);

    static void listen_to_pak_load_result_from_integrity_check_bypass();

    static FaultyTier determine_pak_faulty_tier(std::wstring_view pak_name);
    static FaultyTier detect_if_success_pak_still_suspicious(std::wstring_view pak_name);
    static std::optional<int> extract_patch_version_from_pak_name(std::wstring_view pak_name);
    static bool check_is_stock_patch_pak(std::wstring_view pak_name);

    bool scan_resource_process_parse_and_hook();
    utility::ExhaustionResult scan_for_resource_open_failed_hook(utility::ExhaustionContext& ctx);

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
    safetyhook::MidHook m_resource_open_failed_hook{};
    uint8_t *m_resource_open_failed_addr{};
    uint8_t m_resource_open_failed_register{0};
    CheckFileExistsFunc m_check_file_exist_func{};
    bool m_check_file_exist_func_resolved{ false };

    ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), false) };
    ModInt32::Ptr m_max_recent_files{ ModInt32::create(generate_name("MaxRecentFiles"), 100) };

    static inline FaultyFileDetector* s_instance{nullptr};
    static inline int s_patch_version{0};
    static inline bool s_patch_version_cached{false};
    static inline std::wregex s_extract_patch_version_regex { std::wregex(LR"((.*)patch_(\d+)\.pak)", std::regex::icase | std::regex::ECMAScript) };
    static inline std::vector<std::pair<std::wstring, FaultyTier>> s_buffered_faulties{};

    ValueList m_options{
        *m_enabled,
        *m_max_recent_files,
    };
};