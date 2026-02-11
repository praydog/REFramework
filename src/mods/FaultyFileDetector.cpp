#include "FaultyFileDetector.hpp"
#include "IntegrityCheckBypass.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <utility/String.hpp>
#include <utility/Module.hpp>
#include <utility/Scan.hpp>

#include <safetyhook/mid_hook.hpp>
#include "REFramework.hpp"

// TODO: Probably offset wont change but if it changes then oops
#pragma pack(push, 1)
struct REResource_Via_Raw {
    void *vtable;   // NOTE: 
    wchar_t* path;
    std::uint8_t unk10[0x28];
    bool isInitialized; // 0x38, seems shader/texture resource does not use this variable so dont rely on it
};
#pragma pack(pop)

sdk::Resource* FaultyFileDetector::create_resource_hook_wrapper(sdk::ResourceManager *resource_manager, void* type_info, wchar_t* name) {
    if (s_instance == nullptr) {
        return nullptr;
    }
    return s_instance->create_resource_hook(resource_manager, type_info, name);
}

void FaultyFileDetector::resource_parse_finish_hook_wrapper(safetyhook::Context& ctx) {
    if (s_instance == nullptr) {
        return;
    }
    s_instance->resource_parse_finish_hook(ctx);
}

void FaultyFileDetector::resource_set_argument_hook_wrapper(safetyhook::Context& ctx) {
    if (s_instance == nullptr) {
        return;
    }
    s_instance->resource_set_argument_hook(ctx);
}

FaultyFileDetector::FaultyFileDetector(){
    if (s_instance == nullptr) {
        s_instance = this;
    } else {
        spdlog::warn("[FaultyFileDetector] Multiple instances detected!");
        return;
    }

    m_logger = spdlog::basic_logger_mt("FaultyFileDetector", REFramework::get_persistent_dir("reframework_faulty_files.txt").string(), true);
    
    m_logger->set_level(spdlog::level::info);
    m_logger->flush_on(spdlog::level::info);
    
    m_logger->info("FaultyFileDetector constructed");
}

std::optional<std::string> FaultyFileDetector::on_initialize() {
    auto create_resource_func = sdk::ResourceManager::get_create_resource_function();
    if (create_resource_func == nullptr) {
        m_blocking_error = "Can't find load resource function!";
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);

        return Mod::on_initialize();
    }

    m_create_resource_original = safetyhook::create_inline(
        reinterpret_cast<uint8_t*>(create_resource_func),
        reinterpret_cast<uint8_t*>(&FaultyFileDetector::create_resource_hook_wrapper)
    );

    if (!m_create_resource_original) {
        m_blocking_error = "Failed to hook load resource function!";
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);

        return Mod::on_initialize();
    }

    if (!scan_resource_process_parse_and_hook()) {
        return Mod::on_initialize();
    }

    if (m_blocking_error.has_value()) {
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);
        return Mod::on_initialize();
    }

    spdlog::info("[FaultyFileDetector]: Initialized successfully");
    m_blocking_error = std::nullopt;

    return Mod::on_initialize();
}

bool FaultyFileDetector::scan_resource_process_parse_and_hook() {
    // Future note: related function has a text: ResourceManager::parallelProc, seems like logger or profile marker or something
    static const char *resource_manager_unk_constructor_pattern = "41 56 56 57 53 48 83 EC 28 44 89 C7 48 89 D3 48 89 CE 44 89 41 08 48 C7 41 48 00 00 00 00 48 8D 05 ? ? ? ? 48 89 01 48 8D 51 50 48 8D 05 ? ? ? ? 48 89 41 50 4C 8D 71 58 4C 89 F1";

    auto game = utility::get_executable();
    const auto resource_manager_unk_constructor_ptr = utility::scan(game, resource_manager_unk_constructor_pattern);

    if (!resource_manager_unk_constructor_ptr) {
        m_blocking_error = "Failed to hook parse resource function (anchor to search not found)!";
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);
        return false;
    }

    auto unk_constructor_refs = utility::scan_displacement_references(game, resource_manager_unk_constructor_ptr.value());
    if (unk_constructor_refs.empty()) {
        m_blocking_error = "Failed to hook parse resource function (no references to constructor found)!";
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);
        return false;
    }

    for (auto anchor : unk_constructor_refs) {
        auto probably_call_instr_ptr = reinterpret_cast<std::uint8_t*>(anchor) - 1; // Displacement is one byte ahead
        auto call_instr = utility::decode_one(reinterpret_cast<std::uint8_t*>(probably_call_instr_ptr));

        if (call_instr.has_value() && call_instr->Instruction == ND_INS_CALLNR && call_instr->OperandsCount >= 1 && call_instr->Operands[0].Type == ND_OP_OFFS) {
            auto after_call_ptr = reinterpret_cast<std::uint8_t*>(probably_call_instr_ptr) + call_instr->Length;

            // Find three consecutive calls after the constructor call, all are vtable call at offset 0x20, 0x48 and 0x38
            // call qword ptr [rax + 20h/48h/38h]
            // Second call is the one we want to check its result
            const int first_call_vtable_offset = 0x20;
            const int second_call_vtable_offset = 0x48;
            const int third_call_vtable_offset = 0x38;

            const int max_instructions_search_range = 60;

            int current_calls_found = 0;

            std::uint8_t *parse_call_ptr = nullptr;
            std::uint8_t *parse_call_return_address = nullptr;
            std::uint8_t *start_searching_resource_access_ptr = nullptr;

            for (int i = 0; i < max_instructions_search_range; i++) {
                auto instr = utility::decode_one(after_call_ptr);
                if (!instr.has_value()) {
                    break;
                }

                if (instr->Instruction == ND_INS_CALLNI) {
                    if (instr->OperandsCount >= 1 && instr->Operands[0].Type == ND_OP_MEM) {
                        auto mem_operand = instr->Operands[0].Info.Memory;
                        if (mem_operand.Base == NDR_RAX) {
                            if (current_calls_found == 0 && mem_operand.Disp == first_call_vtable_offset) {
                                // First call found
                                current_calls_found++;
                                start_searching_resource_access_ptr = after_call_ptr + instr->Length;
                            } else if (current_calls_found == 1 && mem_operand.Disp == second_call_vtable_offset) {
                                // Second call found
                                current_calls_found++;
                                parse_call_ptr = after_call_ptr;
                                parse_call_return_address = after_call_ptr + instr->Length;
                            } else if (current_calls_found == 2 && mem_operand.Disp == third_call_vtable_offset) {
                                // Third call found
                                current_calls_found++;
                                break;
                            }
                        }
                    }
                }

                after_call_ptr += instr->Length;
            }

            if (current_calls_found == 3) {
                // Definitively what we are looking for
                if (parse_call_ptr == nullptr || start_searching_resource_access_ptr == nullptr) {
                    spdlog::error("[FaultyFileDetector]: parse_call_ptr is null even though we found all three calls, this should not happen");
                }

                static const int resource_register_search_max_num_instructions = 10;
                std::uint8_t *instr_ptr = reinterpret_cast<std::uint8_t*>(start_searching_resource_access_ptr);
                std::uint8_t *resource_argument_assigned_ptr = nullptr;

                // Very fragile way, but it works for now
                for (int i = 0; i < resource_register_search_max_num_instructions && instr_ptr < parse_call_ptr; i++) {
                    auto instrux = utility::decode_one(instr_ptr);
                    if (instrux && instrux->Instruction == ND_INS_MOV) {
                        bool is_first_operator_first_arg = instrux->OperandsCount >= 1 && instrux->Operands[0].Type == ND_OP_REG && instrux->Operands[0].Info.Register.Reg == NDR_RCX;
                        bool is_second_operator_register = instrux->OperandsCount >= 2 && instrux->Operands[1].Type == ND_OP_REG;

                        if (is_first_operator_first_arg && is_second_operator_register) {
                            resource_argument_assigned_ptr = instr_ptr + instrux->Length;
                            spdlog::info("[FaultyFileDetector]: Found resource argument assign at 0x{:x}, hooking to extract it at: 0x{:x}", (uintptr_t)instr_ptr, (uintptr_t)resource_argument_assigned_ptr);

                            break;
                        }
                    }
                    instr_ptr += instrux ? instrux->Length : 1;
                }

                if (resource_argument_assigned_ptr == nullptr) {
                    spdlog::warn("[FaultyFileDetector]: Failed to find resource argument assign from 0x{:X}", (uintptr_t)parse_call_ptr);
                } else {
                    spdlog::info("[FaultyFileDetector]: All checks passed, hooking resource parse finish function at 0x{:X}", (uintptr_t)parse_call_return_address);

                    // Hook after finish
                    auto hook = safetyhook::create_mid(
                        reinterpret_cast<uint8_t*>(parse_call_return_address),
                        &resource_parse_finish_hook_wrapper
                    );

                    auto argument_set_hook = safetyhook::create_mid(
                        reinterpret_cast<uint8_t*>(resource_argument_assigned_ptr),
                        &resource_set_argument_hook_wrapper
                    );

                    m_resource_parse_finish_hooks.push_back(std::move(hook));
                    m_resource_set_argument_hooks.push_back(std::move(argument_set_hook));
                }
            } else {
                spdlog::warn("[FaultyFileDetector]: Failed to find three consecutive vtable calls after unk constructor call at 0x{:X}", (uintptr_t)anchor);
            }
        } else {
            spdlog::warn("[FaultyFileDetector]: Failed to decode probable call instruction at 0x{:X}", (uintptr_t)probably_call_instr_ptr);
        }
    }

    if (m_resource_parse_finish_hooks.empty()) {
        m_blocking_error = "Failed to hook parse resource function (no valid parse function hooks created)!";
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);
        return false;
    }

    return true;
}

void FaultyFileDetector::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void FaultyFileDetector::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

sdk::Resource* FaultyFileDetector::create_resource_hook(sdk::ResourceManager *resource_manager, void* type_info, wchar_t* name) {
    //spdlog::warn("[FaultyFileDetector]: create_resource_hook called with name: {}", utility::narrow(name ? name : L"(null)"));
    auto original_result = m_create_resource_original.call<sdk::Resource*>(resource_manager, type_info, name);
    if (!m_enabled->value()) {
        return original_result;
    }
    if (original_result == nullptr && name != nullptr) {
        try_add_to_faulty_list(name);
    }
    return original_result;
}

void FaultyFileDetector::resource_set_argument_hook(safetyhook::Context& ctx) {
    if (!m_enabled->value()) {
        return;
    }

    void *resource_ptr = reinterpret_cast<void*>(ctx.rcx);
    std::thread::id thread_id = std::this_thread::get_id();

    {
        std::scoped_lock lock{m_mutex};
        m_resource_by_thread_map[thread_id] = resource_ptr;
    }
}

void FaultyFileDetector::resource_parse_finish_hook(safetyhook::Context& ctx) {
    if (!m_enabled->value()) {
        return;
    }

    // Check parse result
    bool parse_result = ctx.rax & 0x1; // First arg is parse result (0 = fail, 1 = success)

    if (!parse_result) {
        auto thread_id = std::this_thread::get_id();
        REResource_Via_Raw* resource = nullptr;

        {
            std::scoped_lock lock{m_mutex};

            if (!m_resource_by_thread_map.contains(thread_id)) {
                return;
            } else {
                resource = reinterpret_cast<REResource_Via_Raw*>(m_resource_by_thread_map[thread_id]);
            }
        }

        // Get resource pointer from register
        if (resource != nullptr) {
            // Log parsed resource path for debugging
            // spdlog::info("[FaultyFileDetector]: resource_parse_finish_hook called for resource path: {}", utility::narrow(resource->path));

            // Get resource name
            std::wstring_view resource_path(resource->path);
            try_add_to_faulty_list(resource_path);
        }
    }
}

void FaultyFileDetector::try_add_to_faulty_list(std::wstring_view filename) {
    if (filename.empty()) {
        return;
    }

    std::wstring name_wstr{filename};

    bool should_log = false;
    
    {
        std::scoped_lock lock{m_mutex};

        // Only log if this is a new faulty file (prevents spam)
        if (m_faulty_files.find(name_wstr) == m_faulty_files.end()) {
            m_faulty_files.insert(name_wstr);
            
            // Add to recent files for display
            m_recent_faulty_files.push_front(name_wstr);
            
            // Trim recent files to max size
            if (m_recent_faulty_files.size() > m_max_recent_files->value()) {
                while (m_recent_faulty_files.size() > m_max_recent_files->value()) {
                    m_recent_faulty_files.pop_back();
                }
            }

            should_log = true;
        }
    }

    if (should_log) {
        // Log the faulty file to dedicated log file
        if (m_logger) {
            m_logger->info("{}", utility::narrow(name_wstr));
        }
    }
}

void FaultyFileDetector::on_draw_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_FirstUseEver);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_blocking_error.has_value()) {
        ImGui::TextWrapped("Error: %s", m_blocking_error->c_str());
        return;
    }

    std::scoped_lock lock{m_mutex};

    // Display statistics
    ImGui::TextWrapped("Total faulty files encountered: %zu", m_faulty_files.size());

    if (m_faulty_files.empty()) {
        ImGui::TextWrapped("No faulty files detected!");
        return;
    }

    ImGui::TextWrapped("Faulty files detected!");
    ImGui::TextWrapped("See reframework_faulty_files.txt for full list. Use external tool to find out what mod/patch is causing the issue.");

    bool changed = false;

    changed |= m_enabled->draw("Enable Faulty File Detector");
    changed |= m_max_recent_files->draw("Max Recent Files to Display");

    if (ImGui::TreeNode("Recent faulty files")) {
        // Display recent files
        int count = 0;
        const int max_display = m_max_recent_files->value();
        
        for (const auto& file : m_recent_faulty_files) {
            if (count >= max_display) {
                break;
            }
            
            ImGui::TextWrapped("%s", utility::narrow(file).c_str());
            count++;
        }

        if (m_faulty_files.size() > (size_t)count) {
            ImGui::TextWrapped("... and %zu more (see log for full list)", m_faulty_files.size() - count);
        }

        ImGui::TreePop();
    }

    if (changed) {
        g_framework->request_save_config();
    }
}