#include <algorithm>
#include <array>
#include <optional>

#include <spdlog/spdlog.h>
#include <bddisasm.h>

#include "sdk/ResourceManager.hpp"

#include "utility/Module.hpp"
#include "utility/Scan.hpp"
#include "utility/String.hpp"

#include "LooseFileLoader.hpp"
#include "LooseTextureLoader.hpp"
#include "REFramework.hpp"

LooseTextureLoader& LooseTextureLoader::get() {
    return LooseFileLoader::get()->get_texture_loader();
}

void LooseTextureLoader::handle_prepare_enqueue_texture_upload_wrapper(safetyhook::Context& context) {
    get().handle_prepare_enqueue_texture_upload(context);
}

void LooseTextureLoader::handle_start_enqueue_texture_upload_wrapper(safetyhook::Context& context) {
    get().handle_start_enqueue_texture_upload(context);
}

void LooseTextureLoader::handle_path_check_to_open_dstorage_file_wrapper(safetyhook::Context& context) {
    get().handle_path_check_to_open_dstorage_file(context);
}

void LooseTextureLoader::handle_resource_hash_path_wrapper(safetyhook::Context& context) {
    get().handle_resource_hash_path(context);
}

// Single-ref variant: find the function that references `ptr` via displacement.
// Returns the function start of the first external caller (not the function containing ptr itself).
static std::optional<uintptr_t> find_reference_function(HMODULE module, uintptr_t ptr) {
    const auto ref = utility::scan_displacement_reference(module, ptr);
    if (!ref.has_value()) {
        return std::nullopt;
    }

    return utility::find_function_start(ref.value());
}

void LooseTextureLoader::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void LooseTextureLoader::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void LooseTextureLoader::on_draw_ui() {
#if !ENABLE_LOOSE_TEXTURE_LOADER
        return;
#else

    if (ImGui::TreeNode("Loose Texture")) {
        if (m_enabled->draw("Enable")) {
            g_framework->request_save_config();
        }

        if (!m_enabled->value()) {
            return;
        }

        if (m_disable_texture_cache->draw("Disable Texture Cache (force reload)")) {
            g_framework->request_save_config();
        }

        ImGui::TextWrapped(
            "The game caches textures and only reloads them from disk when nothing references them anymore.\n"
            "Enabling this option forces a reload every time, bypassing the cache.\n"
            "WARNING: This creates duplicate texture instances in memory and can be very memory-intensive.\n"
            "Only use this while actively editing textures, and disable it during normal play."
        );

        ImGui::Separator();

        // Show loaded resource counters
        {
            std::lock_guard lock(m_resource_path_counters_mutex);

            ImGui::Text("Unique loose textures loaded: %zu", m_resource_path_counters.size());

            if (ImGui::Button("Reset Counters")) {
                m_resource_path_counters.clear();
                m_recent_resources.clear();
            }

            if (!m_recent_resources.empty() && ImGui::TreeNode("Loaded Loose Textures (recent)")) {
                for (const auto& path : m_recent_resources) {
                    auto it = m_resource_path_counters.find(path);
                    uint64_t count = (it != m_resource_path_counters.end()) ? it->second : 0;
                    ImGui::Text("[%llu] %s", count, utility::narrow(path).c_str());
                }

                if (m_resource_path_counters.size() > m_recent_resources.size()) {
                    ImGui::TextDisabled("... and %zu more", m_resource_path_counters.size() - m_recent_resources.size());
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
#endif
}

void LooseTextureLoader::early_initialize() {
#if ENABLE_LOOSE_TEXTURE_LOADER
    hook_dstorage_path_checks();
    hook_dstorage_enqueue_chain();
    hook_resource_path_hashing();
    find_get_path_to_resource_func();
#endif
}

sdk::ResourceManager* LooseTextureLoader::get_resource_manager() {
    if (m_cached_resource_manager == nullptr) {
        m_cached_resource_manager = sdk::ResourceManager::get();
    }
    return m_cached_resource_manager;
}

void* LooseTextureLoader::get_resource_re_type(ResourceType type) {
    if (m_resource_re_type_cache.find(type) == m_resource_re_type_cache.end()) {
        const auto& type_info = ResourceTypeInfoMap.at(type);
        m_resource_re_type_cache[type] = reframework::get_types()->get(type_info.re_type_name);
    }
    return m_resource_re_type_cache[type];
}

void LooseTextureLoader::hook_dstorage_path_checks() {
    auto game = utility::get_executable();
    auto sub_pak_str = utility::scan_string(game, L".sub_000.pak");

    if (!sub_pak_str) {
        spdlog::error("[LooseTextureLoader]: Could not find sub pak string");
        return;
    }

    auto fnc_refs = utility::scan_displacement_references(game, sub_pak_str.value());
    if (fnc_refs.empty()) {
        spdlog::error("[LooseTextureLoader]: Could not find any references to sub pak string");
        return;
    }

    // For each reference, scan until hitting a call instruction (wcsstr)
    for (auto ref : fnc_refs) {
        uintptr_t wcsstr_call_addr = 0;

        utility::linear_decode((uint8_t*)(ref + 4), 20, [&wcsstr_call_addr](utility::ExhaustionContext& ctx) {
            if (ctx.instrux.Category == ND_CAT_CALL) {
                wcsstr_call_addr = ctx.addr;
                return false;
            }
            return true;
        });

        if (wcsstr_call_addr == 0) {
            continue;
        }

        auto hook = safetyhook::create_mid((void*)wcsstr_call_addr, &LooseTextureLoader::handle_path_check_to_open_dstorage_file_wrapper);
        if (hook) {
            spdlog::info("[LooseTextureLoader]: Hooked wcsstr call at 0x{:X} for DStorage .tex bypass", wcsstr_call_addr);
        } else {
            spdlog::error("[LooseTextureLoader]: Failed to hook wcsstr call at 0x{:X}!", wcsstr_call_addr);
        }
        m_path_check_dstorage_hooks.push_back(std::move(hook));
    }
}

void LooseTextureLoader::hook_dstorage_enqueue_chain() {
    auto game = utility::get_executable();

    // Chain: prepare_enqueue -> enqueue -> open_direct_storage_file
    // Find enqueue via refs to both open_direct_storage_file and "list too long" string
    auto dstorage_file_open_func = find_direct_storage_file_open_function();
    if (!dstorage_file_open_func) {
        spdlog::error("[LooseTextureLoader]: Could not find DirectStorage file open function!");
        return;
    }

    const auto list_too_long_str = utility::scan_string(game, "list too long");
    if (!list_too_long_str) {
        spdlog::error("[LooseTextureLoader]: Could not find 'list too long' string in executable");
        return;
    }

    auto enqueue_func = utility::find_function_with_refs(game, { *dstorage_file_open_func, *list_too_long_str });
    if (!enqueue_func) {
        spdlog::error("[LooseTextureLoader]: Could not find enqueue DirectStorage texture upload function!");
        return;
    }

    auto prepare_enqueue_func = find_reference_function(game, *enqueue_func);
    if (!prepare_enqueue_func) {
        spdlog::error("[LooseTextureLoader]: Could not find prepare enqueue function!");
        return;
    }

    spdlog::info("[LooseTextureLoader]: Found prepare enqueue function at 0x{:X}", *prepare_enqueue_func);

    // Walk up the call chain to find the outermost thunk (if any)
    auto prepare_enqueue_func_thunk = *prepare_enqueue_func;

    for (int depth = 0; depth < 16; depth++) {
        auto parent_func = find_reference_function(game, prepare_enqueue_func_thunk);
        if (!parent_func) {
            break;
        }

        // Stop if the parent has no external references (it's the outermost wrapper)
        auto grandparent_refs = utility::scan_displacement_references(game, *parent_func);
        bool has_external_refs = false;
        for (const auto& ref : grandparent_refs) {
            auto ref_func = utility::find_function_start(ref);
            if (ref_func && *ref_func != *parent_func) {
                has_external_refs = true;
                break;
            }
        }

        prepare_enqueue_func_thunk = *parent_func;
        if (!has_external_refs) {
            break;
        }
    }

    spdlog::info("[LooseTextureLoader]: Found prepare enqueue thunk at 0x{:X}", prepare_enqueue_func_thunk);

    m_prepare_enqueue_texture_upload_hook = safetyhook::create_mid((void*)prepare_enqueue_func_thunk, &LooseTextureLoader::handle_prepare_enqueue_texture_upload_wrapper);
    if (m_prepare_enqueue_texture_upload_hook) {
        spdlog::info("[LooseTextureLoader]: Hooked prepare enqueue at 0x{:X}", prepare_enqueue_func_thunk);
    } else {
        spdlog::error("[LooseTextureLoader]: Failed to hook prepare enqueue at 0x{:X}!", prepare_enqueue_func_thunk);
    }

    m_start_enqueue_texture_upload_hook = safetyhook::create_mid((void*)*enqueue_func, &LooseTextureLoader::handle_start_enqueue_texture_upload_wrapper);
    if (m_start_enqueue_texture_upload_hook) {
        spdlog::info("[LooseTextureLoader]: Hooked start enqueue at 0x{:X}", *enqueue_func);
    } else {
        spdlog::error("[LooseTextureLoader]: Failed to hook start enqueue at 0x{:X}!", *enqueue_func);
    }
}

void LooseTextureLoader::hook_resource_path_hashing() {
    auto resource_create_func = (uintptr_t)sdk::ResourceManager::get_create_resource_function();
    if (resource_create_func == 0) {
        spdlog::error("[LooseTextureLoader]: Could not find ResourceManager::create_resource function!");
        return;
    }

    auto fnc_bounds = utility::determine_function_bounds(resource_create_func);
    if (!fnc_bounds) {
        spdlog::error("[LooseTextureLoader]: Failed to determine bounds for create_resource!");
        return;
    }

    // Known xxhash constants to identify the hash function
    static const std::array<uint64_t, 3> hash_constants = {
        0xC2B2AE3D27D4EB4F,
        0x9E3779B185EBCA87,
        0x27D4EB2F165667C5
    };

    // Collect all direct call sites within create_resource
    struct CallSite { uintptr_t call_addr; uintptr_t target_addr; };
    std::vector<CallSite> call_sites;

    utility::exhaustive_decode((uint8_t*)resource_create_func, fnc_bounds->end - fnc_bounds->start, [&](utility::ExhaustionContext& ctx) -> utility::ExhaustionResult {
        if (ctx.instrux.Instruction == ND_INS_CALLNR) {
            if (auto target = utility::resolve_displacement(ctx.addr)) {
                call_sites.push_back({ ctx.addr, *target });
            }
            return utility::ExhaustionResult::STEP_OVER;
        }
        if (ctx.instrux.Category == ND_CAT_CALL) {
            return utility::ExhaustionResult::STEP_OVER;
        }
        return utility::ExhaustionResult::CONTINUE;
    });

    // For each call target, check if it contains xxhash constants
    uintptr_t hash_call_addr = 0;

    for (const auto& site : call_sites) {
        bool found_constant = false;

        utility::exhaustive_decode((uint8_t*)site.target_addr, 8192, [&](utility::ExhaustionContext& ctx) -> utility::ExhaustionResult {
            if (ctx.instrux.Category == ND_CAT_CALL) {
                return utility::ExhaustionResult::BREAK;
            }
            if (ctx.instrux.Instruction == ND_INS_MOV && ctx.instrux.Operands[1].Type == ND_OP_IMM) {
                auto imm = static_cast<uint64_t>(ctx.instrux.Operands[1].Info.Immediate.Imm);
                for (const auto& constant : hash_constants) {
                    if (imm == constant) {
                        found_constant = true;
                        return utility::ExhaustionResult::BREAK;
                    }
                }
            }
            return utility::ExhaustionResult::CONTINUE;
        });

        if (found_constant) {
            hash_call_addr = site.call_addr;
            spdlog::info("[LooseTextureLoader]: Found resource path hash call at 0x{:X} -> target 0x{:X}", site.call_addr, site.target_addr);
            break;
        }
    }

    if (hash_call_addr == 0) {
        spdlog::error("[LooseTextureLoader]: Failed to find resource path hashing function!");
        return;
    }

    m_resource_hash_path_hook = safetyhook::create_mid((void*)hash_call_addr, &LooseTextureLoader::handle_resource_hash_path_wrapper);
    if (m_resource_hash_path_hook) {
        spdlog::info("[LooseTextureLoader]: Hooked resource path hashing at 0x{:X}", hash_call_addr);
    } else {
        spdlog::error("[LooseTextureLoader]: Failed to hook resource path hashing at 0x{:X}!", hash_call_addr);
    }
}

void LooseTextureLoader::find_get_path_to_resource_func() {
    auto game = utility::get_executable();

    // The function references "%ls/%ls/%ls.%d" format string, and has:
    // - First memory displacement in the function is 0x58 (via MOV/MOVZX second operand)
    // - At least one CMP with immediate 12
    auto format_str = utility::scan_string(game, L"%ls/%ls/%ls.%d");
    if (!format_str) {
        spdlog::error("[LooseTextureLoader]: Could not find path format string");
        return;
    }

    spdlog::info("[LooseTextureLoader]: Found path format string at 0x{:X}", *format_str);

    constexpr int EXPECTED_FIRST_MEM_DISP = 1164;
    constexpr int EXPECTED_SECOND_MEM_DISP = 1136;

    auto format_refs = utility::scan_displacement_references(game, *format_str);

    for (auto ref : format_refs) {
        auto func = utility::find_function_start(ref + 4);
        if (!func) continue;

        auto func_bounds = utility::determine_function_bounds(*func);
        if (!func_bounds) continue;

        bool specific_cmp_with_12_found = false;
        bool specific_second_mov_found = false;

        utility::exhaustive_decode((uint8_t*)*func, func_bounds->end - func_bounds->start, [&](utility::ExhaustionContext& ctx) -> utility::ExhaustionResult {
            if (ctx.instrux.Category == ND_CAT_CALL) {
                return utility::ExhaustionResult::STEP_OVER;
            }

            // Count CMP instructions with immediate 12
            if (!specific_cmp_with_12_found && ctx.instrux.Instruction == ND_INS_CMP) {
                if (ctx.instrux.OperandsCount >= 2) {
                    const auto& op1 = ctx.instrux.Operands[0];
                    const auto& op2 = ctx.instrux.Operands[1];

                    if ((op1.Type == ND_OP_MEM && op1.Info.Memory.Disp == EXPECTED_FIRST_MEM_DISP &&
                         op2.Type == ND_OP_IMM && op2.Info.Immediate.Imm == 12) ||
                        (op1.Type == ND_OP_IMM && op1.Info.Immediate.Imm == 12 &&
                         op2.Type == ND_OP_MEM && op2.Info.Memory.Disp == EXPECTED_FIRST_MEM_DISP)) {
                        specific_cmp_with_12_found = true;
                    }
                }
            }

            if (specific_cmp_with_12_found && !specific_second_mov_found) {
                if (ctx.instrux.Instruction == ND_INS_MOV || ctx.instrux.Instruction == ND_INS_MOVZX) {
                    if (ctx.instrux.OperandsCount >= 2) {
                        const auto& src = ctx.instrux.Operands[1];
                        if (src.Type == ND_OP_MEM && src.Info.Memory.Disp == EXPECTED_SECOND_MEM_DISP) {
                            specific_second_mov_found = true;
                        }
                    }
                }
            }

            if (specific_cmp_with_12_found && specific_second_mov_found) {
                return utility::ExhaustionResult::BREAK;
            }

            return utility::ExhaustionResult::CONTINUE;
        });

        if (specific_cmp_with_12_found && specific_second_mov_found) {
            m_get_native_path_to_resource_func = (GetNativeResourcePath)*func;
            spdlog::info("[LooseTextureLoader]: Found get_path_to_resource at 0x{:X}", *func);
            return;
        }
    }

    spdlog::error("[LooseTextureLoader]: Failed to find get_path_to_resource function!");
}

std::optional<uintptr_t> LooseTextureLoader::find_direct_storage_file_open_function() {
    // This is the GUID of IDStorageFile: {5DE95E7B-5A95-6848-A73C-243B29F4B8DA}
    static const uint8_t dstorage_file_guid[] = {
        0x7B, 0x5E, 0xE9, 0x5D, 0x5A, 0x95, 0x68, 0x48,
        0xA7, 0x3C, 0x24, 0x3B, 0x29, 0xF4, 0xB8, 0xDA
    };

    const auto game = utility::get_executable();
    const auto guid_location = utility::scan_data(game, dstorage_file_guid, sizeof(dstorage_file_guid));

    if (!guid_location) {
        spdlog::error("[LooseTextureLoader]: Could not find DStorageFile GUID in executable");
        return std::nullopt;
    }

    auto func_start = find_reference_function(game, *guid_location);

    if (!func_start) {
        spdlog::error("[LooseTextureLoader]: Could not find function start for GUID reference at 0x{:X}", *guid_location);
        return std::nullopt;
    }

    spdlog::info("[LooseTextureLoader]: Found DirectStorage file open function at 0x{:X}", *func_start);

    return func_start;
}

void LooseTextureLoader::handle_path_check_to_open_dstorage_file(safetyhook::Context& context) {
    if (!m_enabled->value()) return;

    wchar_t *target_path = (wchar_t*)context.rcx;
    wchar_t *search_str = (wchar_t*)context.rdx;
    
    // Search if our path contains .tex extension, if yes, replace rdx with ".tex" to make wcsstr return true
    if (target_path && search_str && wcsstr(target_path, TEX_FILE_EXTENSION) != nullptr) {
        //spdlog::info("[LooseTextureLoader]: Detected loose .tex file, patching to bypass it! Path: {}", utility::narrow(target_path));
        context.rdx = (uint64_t)TEX_FILE_EXTENSION;
    }
}

REPakEntryData* LooseTextureLoader::borrow_pak_entry_data(uintptr_t dstorage_file_ptr) {
    std::lock_guard lock(m_file_info_pool_mutex);

    // Try to reuse a free entry
    for (auto& entry : m_pak_entry_data_pool) {
        if (!entry->in_use) {
            entry->in_use = true;
            entry->file_info.dstorage_file_ptr = dstorage_file_ptr;
            entry->handle_info.pak_data = &entry->file_info;
            entry->handle_info.index_in_pak = FAKE_INVALID_INDEX_IN_PAK;
            return &entry->handle_info;
        }
    }

    // No free entry, allocate a new one
    auto& entry = m_pak_entry_data_pool.emplace_back(std::make_unique<PakEntryDataPool>());
    entry->in_use = true;
    entry->file_info.dstorage_file_ptr = dstorage_file_ptr;
    entry->handle_info.pak_data = &entry->file_info;
    entry->handle_info.index_in_pak = FAKE_INVALID_INDEX_IN_PAK;

    //spdlog::info("[LooseTextureLoader]: Pool grew to {} entries", m_pak_entry_data_pool.size());
    return &entry->handle_info;
}

void LooseTextureLoader::release_pak_entry_data(REPakEntryData* handle_info) {
    if (handle_info == nullptr) {
        return;
    }

    std::lock_guard lock(m_file_info_pool_mutex);

    for (auto& entry : m_pak_entry_data_pool) {
        if (&entry->handle_info == handle_info) {
            entry->in_use = false;
            return;
        }
    }
}

void LooseTextureLoader::handle_prepare_enqueue_texture_upload(safetyhook::Context& context) {
    auto* stream = reinterpret_cast<REUniversalStream*>(context.rcx);
    if (stream == nullptr) {
        return;
    }

    auto pak_entry_data = stream->get_pak_entry_data();
    if (pak_entry_data != nullptr) {
        return; // Not a modded file, let the game handle it normally
    }

    if (stream->dstorage_file_ptr == 0) {
        return;
    }

    pak_entry_data = borrow_pak_entry_data(stream->dstorage_file_ptr);
    stream->set_pak_entry_data(pak_entry_data);
    
    //spdlog::info("[LooseTextureLoader]: Filled REUnkFileInfoFromHandle for modded stream file at 0x{:X}", (uintptr_t)stream);
}

void LooseTextureLoader::handle_start_enqueue_texture_upload(safetyhook::Context& context) {
    auto upload_state = reinterpret_cast<DStorageDataUploadState*>(context.rdx);
    if (upload_state == nullptr) {
        return;
    }

    auto stream = upload_state->stream;
    if (stream == nullptr) {
        return;
    }

    // Check if this stream has a fake file info from our pool
    auto pak_entry_data = stream->get_pak_entry_data();
    if (pak_entry_data == nullptr) {
        return; // Not our stream, ignore
    }

    if (pak_entry_data->index_in_pak != FAKE_INVALID_INDEX_IN_PAK) {
        return; // Not ours
    }

    stream->set_pak_entry_data(nullptr);

    // Return the entry to the pool
    release_pak_entry_data(pak_entry_data);
}

void LooseTextureLoader::handle_resource_hash_path(safetyhook::Context& context) {
    if (!m_enabled->value()) return;

    // rcx: path (wchar_t*)
    // rdx: size of hash (bytes)
    // r8: hash combine
    // Hash function is xxhash
    auto* path = reinterpret_cast<const wchar_t*>(context.rcx);
    const auto size = static_cast<size_t>(context.rdx);

    if (path == nullptr || size == 0) {
        return;
    }

    std::wstring_view path_view{path, size / sizeof(wchar_t)};
    if (path_view.empty()) {
        return;
    }

    ResourceType resource_type = ResourceType::None;

    for (const auto& [type, type_info] : ResourceTypeInfoMap) {
        if (path_view.find(type_info.file_extension) != std::wstring_view::npos) {
            resource_type = type;
            break;
        }
    }

    if (resource_type == ResourceType::None) {
         return;
    }

    bool localize = false;

    if (!path_view.empty()) {
        localize = path[0] == L'@';
        if (localize) {
            path_view.remove_prefix(1); // Remove '@' prefix if present, as the function expects a local resource path
        }
    }

    static thread_local std::wstring path_check_against_loose_file;
    bool path_check_against_loose_file_filled = false;

    if (m_get_native_path_to_resource_func != nullptr) {
        auto texture_type = get_resource_re_type(resource_type);
        
        static constexpr const int MIN_PATH_LENGTH = 1024;

        path_check_against_loose_file.resize(path_view.length() * 2 + MIN_PATH_LENGTH);
        path_check_against_loose_file_filled = m_get_native_path_to_resource_func(
            get_resource_manager(),
            path_check_against_loose_file.data(),
            path_view.data(),
            texture_type,
            localize
        );
    }

    if (!path_check_against_loose_file_filled) {
        // Fallback to our own, combine "natives/STM" with path
        path_check_against_loose_file = DEFAULT_ROOT_RESOURCE_PATH + std::wstring(path_view);
    }

    auto& loose_file_loader = LooseFileLoader::get();
    if (loose_file_loader == nullptr || !loose_file_loader->can_loosely_load_file(path_check_against_loose_file.c_str())) {
        return;
    }

    // Get or increment the counter for this path
    uint64_t counter{};
    {
        std::lock_guard lock(m_resource_path_counters_mutex);
        std::wstring key{path_view.data(), path_view.size()};
        counter = m_resource_path_counters[key]++;

        // Update recent list: remove old position if present, push to front
        auto it = std::find(m_recent_resources.begin(), m_recent_resources.end(), key);
        if (it != m_recent_resources.end()) {
            m_recent_resources.erase(it);
        }
        m_recent_resources.insert(m_recent_resources.begin(), std::move(key));
        if (m_recent_resources.size() > MAX_RECENT_DISPLAY) {
            m_recent_resources.pop_back();
        }
    }

    if (!m_disable_texture_cache->value()) {
        return; // Still count, but don't modify the hash path
    }

    // Build the modified path in a thread-local buffer to avoid allocations visible to other threads
    static thread_local std::wstring tls_modified_path{};
    tls_modified_path.assign(path_view.data(), path_view.size());
    tls_modified_path += std::to_wstring(counter);

    context.rcx = reinterpret_cast<uintptr_t>(tls_modified_path.c_str());
    context.rdx = tls_modified_path.size() * sizeof(wchar_t);
}
