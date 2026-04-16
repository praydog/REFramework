#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "Mod.hpp"
#include "sdk/ReClass_LooseTextureLoader_Internal.hpp"

#include <safetyhook.hpp>

#ifdef REFRAMEWORK_UNIVERSAL
// Universal build targets all games; the TDB>=81 requirement is enforced at
// runtime inside early_initialize/on_draw_ui via sdk::GameIdentity.
#define ENABLE_LOOSE_TEXTURE_LOADER 1
#elif TDB_VER >= 81
#define ENABLE_LOOSE_TEXTURE_LOADER 1
#else
#define ENABLE_LOOSE_TEXTURE_LOADER 0
#endif

namespace sdk { class ResourceManager; }

typedef bool (*GetNativeResourcePath)(sdk::ResourceManager *resource_manager, wchar_t *dest_path_buffer, const wchar_t *local_resource_path, void *resource_type, bool localize);

// Component class owned by LooseFileLoader — not a standalone Mod.
class LooseTextureLoader {
public:
    enum class ResourceType {
        None,
        Texture,
        Mesh,
        Material
    };

    struct ResourceTypeInfo {
        ResourceType type;
        std::wstring file_extension;
        std::string re_type_name;
    };

    static inline const std::map<ResourceType, ResourceTypeInfo> ResourceTypeInfoMap = {
        { ResourceType::Texture, { ResourceType::Texture, L".tex", "via.render.TextureResource" } },
    };

public:
    void on_config_load(const utility::Config& cfg);
    void on_config_save(utility::Config& cfg);
    void on_draw_ui();

    bool is_enabled() const { return m_enabled->value(); }

    static LooseTextureLoader& get();
    void early_initialize();

private:
    static constexpr const char* CONFIG_PREFIX = "LooseFileLoader_";
    static std::string generate_name(std::string_view name) { return std::string{CONFIG_PREFIX} + name.data(); }

    static constexpr const wchar_t *DEFAULT_ROOT_RESOURCE_PATH = L"natives/STM/";
    static constexpr const wchar_t *TEX_FILE_EXTENSION = L".tex.";

    static constexpr size_t MAX_RECENT_DISPLAY = 50;

private:
    // Lazy-cached lookups
    sdk::ResourceManager* get_resource_manager();
    void* get_resource_re_type(ResourceType type);

    void hook_dstorage_path_checks();
    void hook_dstorage_enqueue_chain();
    void hook_resource_path_hashing();
    void find_get_path_to_resource_func();

    std::optional<uintptr_t> find_direct_storage_file_open_function();
    void handle_path_check_to_open_dstorage_file(safetyhook::Context& context);
    void handle_prepare_enqueue_texture_upload(safetyhook::Context& context);
    void handle_start_enqueue_texture_upload(safetyhook::Context& context);
    REPakEntryData* borrow_pak_entry_data(uintptr_t dstorage_file_ptr);
    void release_pak_entry_data(REPakEntryData* handle_info);
    void handle_resource_hash_path(safetyhook::Context& context);

    // Static wrappers for safetyhook callbacks (must be plain function pointers)
    static void handle_path_check_to_open_dstorage_file_wrapper(safetyhook::Context& context);
    static void handle_prepare_enqueue_texture_upload_wrapper(safetyhook::Context& context);
    static void handle_start_enqueue_texture_upload_wrapper(safetyhook::Context& context);
    static void handle_resource_hash_path_wrapper(safetyhook::Context& context);

private:
    // Hooks
    std::vector<safetyhook::MidHook> m_path_check_dstorage_hooks{};
    safetyhook::MidHook m_prepare_enqueue_texture_upload_hook{};
    safetyhook::MidHook m_start_enqueue_texture_upload_hook{};
    safetyhook::MidHook m_resource_hash_path_hook{};

    struct PakEntryDataPool {
        REPakEntryData handle_info{};
        REPakData file_info{};
        bool in_use{false};
    };

    std::vector<std::unique_ptr<PakEntryDataPool>> m_pak_entry_data_pool{};
    std::mutex m_file_info_pool_mutex{};

    GetNativeResourcePath m_get_native_path_to_resource_func{nullptr};

    std::map<std::wstring, uint64_t> m_resource_path_counters{};
    std::vector<std::wstring> m_recent_resources{};  // Most recent first, capped at MAX_RECENT_DISPLAY
    std::mutex m_resource_path_counters_mutex{};

    // Config
    ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled"), true) };
    ModToggle::Ptr m_disable_texture_cache{ ModToggle::create(generate_name("DisableTextureCache"), false) };

    std::vector<std::reference_wrapper<IModValue>> m_options{
        *m_enabled,
        *m_disable_texture_cache,
    };

    // Lazy-cached values
    sdk::ResourceManager* m_cached_resource_manager{nullptr};
    std::map<ResourceType, void*> m_resource_re_type_cache{};
};
