#include <sdk/RETypeDB.hpp>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "LooseFileLoader.hpp"

LooseFileLoader* g_loose_file_loader{nullptr};

std::shared_ptr<LooseFileLoader>& LooseFileLoader::get() {
    static auto instance = std::shared_ptr<LooseFileLoader>(new LooseFileLoader());
    return instance;
}

std::optional<std::string> LooseFileLoader::on_initialize() {
    g_loose_file_loader = this;
    return Mod::on_initialize();
}

void LooseFileLoader::on_frame() {
    if (!m_attempted_hook && m_enabled->value()) {
        hook();
    }
}

void LooseFileLoader::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    /*if (!m_attempted_hook && m_enabled->value()) {
        hook();
    }*/
}

void LooseFileLoader::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void LooseFileLoader::on_draw_ui() {
    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_attempted_hook && !m_hook_success) {
        ImGui::TextWrapped("Failed to hook successfully. This mod will not work.");
        return;
    }

    if (m_enabled->draw("Enable Loose File Loader")) {
    }

    if (m_hook_success) {
        ImGui::TextWrapped("Files encountered: %d", m_files_encountered);
        ImGui::TextWrapped("Loose files loaded: %d", m_loose_files_loaded);
    }
}

void LooseFileLoader::hook() {
    if (m_attempted_hook) {
        return;
    }

    spdlog::info("[LooseFileLoader] Attempting to find path_to_hash");

    m_attempted_hook = true;

    const auto tdb = sdk::RETypeDB::get();

    if (tdb == nullptr) {
        spdlog::error("[LooseFileLoader] Failed to get type database");
        return;
    }

    const auto via_io_file = tdb->find_type("via.io.file");

    if (via_io_file == nullptr) {
        spdlog::error("[LooseFileLoader] Failed to find via.io.file");
        return;
    }

    // "exist" and "exists" are 2 different functions. one searches in pak and other doesnt.
    const auto exists_fn = via_io_file->get_method("exists(System.String)");

    if (exists_fn == nullptr) {
        spdlog::error("[LooseFileLoader] Failed to find via.io.file.exists(System.String)");
        return;
    }

    const auto exists_ptr = exists_fn->get_function();

    if (exists_ptr == nullptr) {
        spdlog::error("[LooseFileLoader] Failed to get function pointer for via.io.file.exists(System.String)");
        return;
    }

    std::optional<uintptr_t> candidate{};
    const auto game_module = utility::get_executable();
    const auto game_module_size = utility::get_module_size(game_module).value_or(0);
    const auto game_module_end = (uintptr_t)game_module + game_module_size;

    utility::exhaustive_decode((uint8_t*)exists_ptr, 500, [&](utility::ExhaustionContext& ctx) -> utility::ExhaustionResult {
        if (candidate) {
            return utility::ExhaustionResult::BREAK;
        }

        // We do not care about the address if it is not in the game module, e.g. inside of kernel32.dll
        if (ctx.addr < (uintptr_t)game_module || ctx.addr > game_module_end) {
            return utility::ExhaustionResult::BREAK;
        }

        // This means we have just entered call or something
        if (ctx.branch_start == ctx.addr) {
            if (auto bs = utility::find_string_reference_in_path(ctx.branch_start, L"\\", false); bs.has_value()) {
                spdlog::info("[LooseFileLoader] Found a backslash reference at {:x}", bs->addr);

                // Now check if murmur hash constant is in the path
                if (utility::find_pattern_in_path((uint8_t*)ctx.branch_start, 500, true, "? ? 6B CA EB 85")) {
                    candidate = ctx.branch_start;
                    spdlog::info("[LooseFileLoader] Found path_to_hash candidate at {:x}", *candidate);
                    return utility::ExhaustionResult::BREAK;
                }
            }
        }

        return utility::ExhaustionResult::CONTINUE;
    });

    if (!candidate) {
        spdlog::error("[LooseFileLoader] Failed to find path_to_hash candidate, cannot continue");
        return;
    }

    m_path_to_hash_hook = std::make_unique<FunctionHook>(candidate.value(), (uintptr_t)&path_to_hash_hook);

    if (!m_path_to_hash_hook->create()) {
        spdlog::error("[LooseFileLoader] Failed to hook path_to_hash");
        return;
    }

    m_hook_success = true;
}

uint64_t LooseFileLoader::path_to_hash_hook(const wchar_t* path) {
    ++g_loose_file_loader->m_files_encountered;
    const auto enabled = g_loose_file_loader->m_enabled->value();

    if (enabled && path != nullptr && path[0] != L'\0') {
        //spdlog::info("[LooseFileLoader] path_to_hash_hook called with path: {}", utility::narrow(path));

        if (std::filesystem::exists(path)) {
            ++g_loose_file_loader->m_loose_files_loaded;
            return 4294967296;
        }
    }


    const auto og = g_loose_file_loader->m_path_to_hash_hook->get_original<decltype(path_to_hash_hook)>();
    const auto result = og(path);

    return result;
}