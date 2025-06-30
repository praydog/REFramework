#include <sdk/RETypeDB.hpp>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>
#include "REFramework.hpp"

#include <spdlog/sinks/basic_file_sink.h>

#include "LooseFileLoader.hpp"

LooseFileLoader* g_loose_file_loader{nullptr};

LooseFileLoader::LooseFileLoader() 
{
    g_loose_file_loader = this;

    m_logger = spdlog::basic_logger_mt("LooseFileLoader", REFramework::get_persistent_dir("reframework_accessed_files.txt").string(), true);
    m_loose_file_logger = spdlog::basic_logger_mt("LooseFileLoader2", REFramework::get_persistent_dir("reframework_loose_files.txt").string(), true);

    m_logger->set_level(spdlog::level::info);
    m_logger->flush_on(spdlog::level::info);

    m_logger->info("LooseFileLoader constructed");

    m_loose_file_logger->set_level(spdlog::level::info);
    m_loose_file_logger->flush_on(spdlog::level::info);

    m_loose_file_logger->info("LooseFileLoader constructed");
}

std::shared_ptr<LooseFileLoader>& LooseFileLoader::get() {
    static auto instance = std::shared_ptr<LooseFileLoader>(new LooseFileLoader());
    return instance;
}

std::optional<std::string> LooseFileLoader::on_initialize() {
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

    auto clear_existence_cache = [&]() {
        std::unique_lock _{m_files_on_disk_mutex};
        m_files_on_disk.clear();
        m_seen_files.clear();
        m_cache_hits = 0;
        m_uncached_hits = 0;
    };

    if (m_enabled->draw("Enable Loose File Loader")) {
        clear_existence_cache();
    }

    if (m_hook_success) {
        ImGui::TextWrapped("Files encountered: %d", m_files_encountered);
        ImGui::TextWrapped("Loose files loaded: %d", m_loose_files_loaded);

        if (ImGui::Button("Clear stats")) {
            m_files_encountered = 0;
            m_loose_files_loaded = 0;

            std::unique_lock _{m_mutex};
            m_recent_accessed_files.clear();
            m_recent_loose_files.clear();
            m_all_accessed_files.clear();
            m_all_loose_files.clear();
        }

        if (ImGui::TreeNode("Debug")) {
            ImGui::Checkbox("Enable file cache", &m_enable_file_cache);
            ImGui::TextWrapped("Cache hits: %d", m_cache_hits);
            ImGui::TextWrapped("Uncached hits: %d", m_uncached_hits);

            if (ImGui::Button("Clear existence cache")) {
                clear_existence_cache();
            }

            ImGui::TreePop();
        }

        m_log_accessed_files->draw("Log accessed files");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Logs all accessed files to <game_dir>/reframework_accessed_files.txt");
            ImGui::EndTooltip();
        }

        m_log_loose_files->draw("Log loose files");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Logs loaded loose files to <game_dir>/reframework_loose_files.txt");
            ImGui::EndTooltip();
        }

        ImGui::Checkbox("Show recent files", &m_show_recent_files);

        if (m_show_recent_files) {
            std::shared_lock _{m_mutex};

            if (ImGui::TreeNode("Recent accessed files")) {
                for (const auto& file : m_recent_accessed_files) {
                    ImGui::TextWrapped("%s", utility::narrow(file).c_str());
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Recent loose files")) {
                for (const auto& file : m_recent_loose_files) {
                    ImGui::TextWrapped("%s", utility::narrow(file).c_str());
                }

                ImGui::TreePop();
            }
        }
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

    auto initial_candidates = [&]() -> std::vector<uintptr_t> {
        sdk::RETypeDefinition* via_io_file = nullptr;

        // We need to look for via.io.file manually because LooseFileLoader gets loaded extremely early
        // meaning VM stuff may not work correctly
        for (auto i = 0; i < tdb->get_num_types(); ++i) {
            const auto t = tdb->get_type(i);

            if (t == nullptr || t->get_name() == nullptr) {
                continue;
            }

            if (std::string_view{t->get_name()} == "file") {
                if (t->get_declaring_type() != nullptr && std::string_view{t->get_declaring_type()->get_name()} == "io") {
                    via_io_file = t;
                    break;
                }
            }
        }

        if (via_io_file == nullptr) {
            spdlog::error("[LooseFileLoader] Failed to find via.io.file");
            return {};
        }

        spdlog::info("[LooseFileLoader] Found via.io.file");

        std::vector<uintptr_t> candidates{};

        // Same reason as above, manually loop through methods because VM stuff may not work correctly
        for (auto& m : via_io_file->get_methods()) {
            if (m.get_name() == nullptr) {
                continue;
            }

            if (std::string_view{m.get_name()} == "exists") {
                if (auto func = m.get_function(); func != nullptr) {
                    candidates.push_back((uintptr_t)func);
                }
            }
        }

        if (candidates.empty()) {
            spdlog::error("[LooseFileLoader] Failed to find via.io.file.exists methods");
        }

        return candidates;
    }();

    const auto game_module = utility::get_executable();
    const auto game_module_size = utility::get_module_size(game_module).value_or(0);
    const auto game_module_end = (uintptr_t)game_module + game_module_size;

    std::optional<uintptr_t> candidate{};

    auto check_fn = [&](uintptr_t exists_ptr) {
        spdlog::info("[LooseFileLoader] Scanning for path_to_hash candidate at {:x}", exists_ptr);

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
    };

    if (initial_candidates.empty()) {
        // Basically what we're doing here is finding an initial "mov r8d, 800h"
        // and then finding a "mov r8d, 400h" in the function, as well as another "mov r8d, 800h"
        // I call this a landmark scan, where we find a sequence of instructions that are unique to the function
        // but they don't need to be near each other, they just need to be in the same function.
        // Some other giveaways of the function are the uses of the UTF-16 backslash characters
        // and the two calls to murmur hash functions at the end (these can be found in System.String's GetHashCode)
        const std::string start_seq {"41 B8 00 08 00 00"}; // mov r8d, 800h
        std::vector<std::string> patterns_in_function {
            "BA 00 04 00 00",
            "41 B8 00 08 00 00",
        };

        std::unordered_set<uintptr_t> analyzed_fns{};

        for (auto new_candidate = utility::find_landmark_sequence(game_module, start_seq, patterns_in_function, false);
            new_candidate.has_value();
            new_candidate = utility::find_landmark_sequence(new_candidate->addr + new_candidate->instrux.Length, game_module_end - (new_candidate->addr + 1), start_seq, patterns_in_function, false)) 
        {
            const auto fn_start = utility::find_function_start_with_call(new_candidate->addr);

            if (!fn_start.has_value()) {
                spdlog::error("[LooseFileLoader] Failed to find path_to_hash candidate's start, cannot continue");
                continue;
            }

            if (analyzed_fns.contains(fn_start.value())) {
                continue;
            }

            analyzed_fns.insert(fn_start.value());

            check_fn(*fn_start);

            if (candidate) {
                break;
            }
        }
    } else {
        for (const auto& c : initial_candidates) {
            check_fn(c);

            if (candidate) {
                break;
            }
        }
    }

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

static thread_local std::chrono::steady_clock::time_point g_last_time_logged_safe_exists{};

bool safe_exists(const wchar_t* path) try {
    return std::filesystem::exists(path);
} catch (const std::filesystem::filesystem_error& e) {
    if (std::chrono::steady_clock::now() - g_last_time_logged_safe_exists > std::chrono::seconds(1)) {
        spdlog::error("[LooseFileLoader] Filesystem error in safe_exists: {}", e.what());
        g_last_time_logged_safe_exists = std::chrono::steady_clock::now();
    }
    return false;
} catch (const std::exception& e) {
    if (std::chrono::steady_clock::now() - g_last_time_logged_safe_exists > std::chrono::seconds(1)) {
        spdlog::error("[LooseFileLoader] Exception in safe_exists: {}", e.what());
        g_last_time_logged_safe_exists = std::chrono::steady_clock::now();
    }
    return false;
} catch (...) {
    if (std::chrono::steady_clock::now() - g_last_time_logged_safe_exists > std::chrono::seconds(1)) {
        spdlog::error("[LooseFileLoader] Unknown exception in safe_exists!");
        g_last_time_logged_safe_exists = std::chrono::steady_clock::now();
    }
    return false;
}

bool LooseFileLoader::handle_path(const wchar_t* path, size_t hash) {
    if (path == nullptr || path[0] == L'\0') {
        return false;
    }

    ++m_files_encountered;

    if (m_show_recent_files) {
        std::unique_lock _{m_mutex};

        m_all_accessed_files.insert(path);

        m_recent_accessed_files.push_front(path);

        if (m_recent_accessed_files.size() > 100) {
            m_recent_accessed_files.pop_back();
        }
    }

    const auto enabled = m_enabled->value();

    //spdlog::info("[LooseFileLoader] path_to_hash_hook called with path: {}", utility::narrow(path));

    if (enabled) {
        bool exists_in_cache{false};
        bool exists_on_disk{false};

        if (m_enable_file_cache) {
            // Intended to get rid of mutex usage which can be a bottleneck
            static thread_local std::unordered_set<size_t> files_on_disk_local{};
            static thread_local std::unordered_set<size_t> seen_files_local{};

            {
                // No need to lock a mutex as these are thread_local
                exists_on_disk = files_on_disk_local.contains(hash);
                exists_in_cache = exists_on_disk || seen_files_local.contains(hash);
            }

            if (!exists_in_cache) {
                // TODO: refine this with mixed shared and unique locks
                // This shouldnt be a huge performance issue for now
                std::unique_lock _{m_files_on_disk_mutex};

                // Purpose of this is to only hit the disk once per unique file
                if (m_files_on_disk.contains(hash) || safe_exists(path)) {
                    m_files_on_disk.insert(hash); // Global
                    files_on_disk_local.insert(hash); // Thread local
                    exists_on_disk = true;
                }

                if (m_log_accessed_files->value()) {
                    m_logger->info("{}", utility::narrow(path));
                }

                if (exists_on_disk && m_log_loose_files->value()) {
                    m_loose_file_logger->info("{}", utility::narrow(path));
                }

                m_seen_files.insert(hash); // Global
                seen_files_local.insert(hash); // Thread local
                ++m_uncached_hits;
            } else {
                ++m_cache_hits;
            }
        } else {
            exists_on_disk = safe_exists(path);
            ++m_uncached_hits;
        }

        if (m_show_recent_files && exists_on_disk) {
            std::unique_lock _{m_mutex};

            m_all_loose_files.insert(path);
            m_recent_loose_files.push_front(path);

            if (m_recent_loose_files.size() > 100) {
                m_recent_loose_files.pop_back();
            }
        }

        if (exists_on_disk) {
            ++g_loose_file_loader->m_loose_files_loaded;
            return true;
        }
    }

    return false;
}

#if TDB_VER > 67
uint64_t LooseFileLoader::path_to_hash_hook(const wchar_t* path) {
#else
uint64_t LooseFileLoader::path_to_hash_hook(void* This, const wchar_t* path) {
#endif
    const auto og = g_loose_file_loader->m_path_to_hash_hook->get_original<decltype(path_to_hash_hook)>();

#if TDB_VER > 67
    const auto result = og(path);
#else
    const auto result = og(This, path);
#endif

    // true to skip.
    if (g_loose_file_loader->handle_path(path, result)) {
#if TDB_VER > 67
        return 4294967296;
#else
        return 0xFFFFFFFF;
#endif
    }

    return result;
}