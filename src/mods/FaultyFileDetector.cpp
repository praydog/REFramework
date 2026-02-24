#include "FaultyFileDetector.hpp"
#include "IntegrityCheckBypass.hpp"

#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <utility/String.hpp>
#include <utility/Module.hpp>
#include <utility/Scan.hpp>

#include <safetyhook/mid_hook.hpp>
#include "REFramework.hpp"

std::shared_ptr<FaultyFileDetector> g_faulty_detector_instance = nullptr;

// TODO: Probably offset wont change but if it changes then oops
#pragma pack(push, 1)
struct REResource_Via_Raw {
    void *vtable;   // NOTE: 
    wchar_t* path;
    std::uint8_t unk10[0x28];
    bool isInitialized; // 0x38, seems shader/texture resource does not use this variable so dont rely on it
};
#pragma pack(pop)

const char *faulty_reason_to_string(FaultyFileDetector::FaultyReason reason) {
    switch (reason) {
        case FaultyFileDetector::FaultyReason::Unknown:
            return "Unknown reason";
        case FaultyFileDetector::FaultyReason::MissingFile:
            return "Missing file";
        case FaultyFileDetector::FaultyReason::Invalid:
            return "Invalid file";
        case FaultyFileDetector::FaultyReason::ShouldBeEncrypted:
            return "PAK should be encrypted";
        default:
            return "Unknown reason";
    }
}

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

void FaultyFileDetector::resource_parse_open_stream_failed_hook_wrapper(safetyhook::Context& ctx) {
    if (s_instance == nullptr) {
        return;
    }
    s_instance->resource_parse_open_stream_failed_hook(ctx);
}

template <typename T>
T* get_register_value(safetyhook::Context& ctx, uint8_t reg) {
    switch (reg) {
        case NDR_RAX: return reinterpret_cast<T*>(ctx.rax);
        case NDR_RBX: return reinterpret_cast<T*>(ctx.rbx);
        case NDR_RCX: return reinterpret_cast<T*>(ctx.rcx);
        case NDR_RDX: return reinterpret_cast<T*>(ctx.rdx);
        case NDR_RSI: return reinterpret_cast<T*>(ctx.rsi);
        case NDR_RDI: return reinterpret_cast<T*>(ctx.rdi);
        case NDR_RBP: return reinterpret_cast<T*>(ctx.rbp);
        case NDR_RSP: return reinterpret_cast<T*>(ctx.rsp);
        case NDR_R8:  return reinterpret_cast<T*>(ctx.r8);
        case NDR_R9:  return reinterpret_cast<T*>(ctx.r9);
        case NDR_R10: return reinterpret_cast<T*>(ctx.r10);
        case NDR_R11: return reinterpret_cast<T*>(ctx.r11);
        case NDR_R12: return reinterpret_cast<T*>(ctx.r12);
        case NDR_R13: return reinterpret_cast<T*>(ctx.r13);
        case NDR_R14: return reinterpret_cast<T*>(ctx.r14);
        case NDR_R15: return reinterpret_cast<T*>(ctx.r15);
        default: return nullptr;
    }
}

FaultyFileDetector::FaultyFileDetector() {
    if (s_instance == nullptr) {
        s_instance = this;
    } else {
        spdlog::warn("[FaultyFileDetector] Multiple instances detected!");
        return;
    }

    m_logger = spdlog::basic_logger_mt("FaultyFileDetector", REFramework::get_persistent_dir("reframework_faulty_files.txt").string(), true);
    
    m_logger->set_level(spdlog::level::info);
    m_logger->flush_on(spdlog::level::info);

    m_logger->set_pattern("\"%l\" %v");
}

std::optional<std::string> FaultyFileDetector::on_initialize() {
    initialize_impl();
    return Mod::on_initialize();
}

void FaultyFileDetector::initialize_impl() {
    if (m_initialized) {
        return;
    }

    m_initialized = true;

    auto create_resource_func = sdk::ResourceManager::get_create_resource_function();
    if (create_resource_func == nullptr) {
        m_blocking_error = "Can't find load resource function!";
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);
    }

    m_create_resource_original = safetyhook::create_inline(
        reinterpret_cast<uint8_t*>(create_resource_func),
        reinterpret_cast<uint8_t*>(&FaultyFileDetector::create_resource_hook_wrapper)
    );

    if (!m_create_resource_original) {
        m_blocking_error = "Failed to hook load resource function!";
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);
    }

    if (!scan_resource_process_parse_and_hook()) {
        return;
    }

    if (m_blocking_error.has_value()) {
        spdlog::error("[FaultyFileDetector] {}", *m_blocking_error);
        return;
    }

    spdlog::info("[FaultyFileDetector]: Initialized successfully");
    m_blocking_error = std::nullopt;
}

utility::ExhaustionResult FaultyFileDetector::scan_for_resource_open_failed_hook(utility::ExhaustionContext& ctx) {
    if (ctx.instrux.Instruction == ND_INS_CALLNI || ctx.instrux.Instruction == ND_INS_CALLNR || ctx.instrux.Instruction == ND_INS_CALLFD || ctx.instrux.Instruction == ND_INS_CALLFI) {
        return utility::ExhaustionResult::STEP_OVER;
    }

    if (ctx.instrux.Instruction == ND_INS_MOV && ctx.instrux.OperandsCount >= 2 && 
        ctx.instrux.Operands[0].Type == ND_OP_MEM && ctx.instrux.Operands[0].Info.Memory.Disp == 0x3C &&
        ctx.instrux.Operands[1].Type == ND_OP_IMM && ((ctx.instrux.Operands[1].Info.Immediate.Imm & 0xFFFFFFFF) == 0xFFFFFFFF)) {
        m_resource_open_failed_addr = (std::uint8_t*)(ctx.addr);
        m_resource_open_failed_register = ctx.instrux.Operands[0].Info.Memory.Base;

        return utility::ExhaustionResult::BREAK;
    }

    return utility::ExhaustionResult::CONTINUE;
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

    std::unordered_set<uintptr_t> process_func_candidates;

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

                    auto fn = utility::find_function_start((uintptr_t)parse_call_return_address);

                    if (fn.has_value() && !process_func_candidates.contains(fn.value())) {
                        process_func_candidates.insert(fn.value());
                    }
                }
            } else {
                spdlog::warn("[FaultyFileDetector]: Failed to find three consecutive vtable calls after unk constructor call at 0x{:X}", (uintptr_t)anchor);
            }
        } else {
            spdlog::warn("[FaultyFileDetector]: Failed to decode probable call instruction at 0x{:X}", (uintptr_t)probably_call_instr_ptr);
        }
    }

    // Search for function that get the next resource to process. That function also checks for the validity of resource stream
    const wchar_t *resource_path_format = L"%ls/%ls/%ls.%d";

    auto str_offset = utility::scan_string(game, resource_path_format, true);
    if (str_offset.has_value()) {
        for (auto candidate_fn_addr : process_func_candidates) {
            auto possible_get_next_resource_to_process_call = utility::find_encapsulating_function_disp(candidate_fn_addr, str_offset.value());
            if (possible_get_next_resource_to_process_call.has_value()) {
                // TODO: For now only one exists, but if multiple exists we should hook them all, just need to figure out how to differentiate them
                spdlog::info("[FaultyFileDetector]: Found possible get next resource to process function at 0x{:X}", possible_get_next_resource_to_process_call.value());

                // Follow the failure function
                const int scan_resource_path_usage_length = 1024;
                auto using_resource_path_offset = utility::scan_displacement_reference(possible_get_next_resource_to_process_call.value(), scan_resource_path_usage_length, str_offset.value());

                std::uint8_t *resource_stream_failed_offset = nullptr;

                if (using_resource_path_offset.has_value()) {
                    // Skip the displacement value
                    auto next_instr = using_resource_path_offset.value() + 4;

                    const int scan_resource_stream_failed_length = 2048;
                    utility::exhaustive_decode((uint8_t*)next_instr, scan_resource_stream_failed_length, std::bind(&FaultyFileDetector::scan_for_resource_open_failed_hook, this, std::placeholders::_1));
                }

                if (m_resource_open_failed_addr) {
                    spdlog::info("[FaultyFileDetector]: Found resource stream failed check at 0x{:X}, hooking to detect failed resource stream", (uintptr_t)m_resource_open_failed_addr);

                    auto hook = safetyhook::create_mid(
                        m_resource_open_failed_addr,
                        &resource_parse_open_stream_failed_hook_wrapper
                    );

                    m_resource_open_failed_hook = std::move(hook);
                    break;
                } else {
                    spdlog::warn("[FaultyFileDetector]: Failed to find resource stream failed check after using resource path at 0x{:X}", (uintptr_t)using_resource_path_offset.value());
                }
            }
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
        try_add_to_faulty_list(name, FaultyTier::Severe, FaultyReason::MissingFile);
    }
    return original_result;
}

void FaultyFileDetector::resource_parse_open_stream_failed_hook(safetyhook::Context& ctx) {
    if (!m_enabled->value()) {
        return;
    }

    REResource_Via_Raw* resource = get_register_value<REResource_Via_Raw>(ctx, m_resource_open_failed_register);

    if (resource) {
        try_add_to_faulty_list(resource->path, FaultyTier::Severe, FaultyReason::MissingFile);
    }
}

void FaultyFileDetector::resource_set_argument_hook(safetyhook::Context& ctx) {
    if (!m_enabled->value()) {
        return;
    }

    void *resource_ptr = reinterpret_cast<void*>(ctx.rcx);
    std::thread::id thread_id = std::this_thread::get_id();

    REResource_Via_Raw* resource = reinterpret_cast<REResource_Via_Raw*>(resource_ptr);

    //spdlog::info("[FaultyFileDetector]: resource_set_argument_hook called for resource path: {}", utility::narrow(resource->path));

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
            // Get resource name
            std::wstring_view resource_path(resource->path);
            try_add_to_faulty_list(resource_path, FaultyTier::Severe, FaultyReason::Invalid);
        }
    }
}

void FaultyFileDetector::try_add_to_faulty_list(std::wstring_view filename, FaultyTier tier, FaultyReason reason) {
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
            
            // Add to recent files for this reason
            auto& reason_deque = m_recent_faulty_files_by_reason[reason];
            reason_deque.push_front(name_wstr);
            
            // Trim recent files to max size for this reason
            if (reason_deque.size() > m_max_recent_files->value()) {
                while (reason_deque.size() > m_max_recent_files->value()) {
                    reason_deque.pop_back();
                }
            }

            should_log = true;
        }
    }

    if (should_log) {
        // Log the faulty file to dedicated log file
        auto log_msg = std::format("{} \"{}\" \"{}\"", (int)reason, faulty_reason_to_string(reason), utility::narrow(name_wstr));

        if (m_logger) {
            switch (tier) {
                case FaultyTier::Warning:
                    m_logger->warn("{}", log_msg);
                    break;
                case FaultyTier::Severe:
                    m_logger->error("{}", log_msg);
                    break;
                default:
                    m_logger->info("{}", log_msg);
                    break;
            }
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
    ImGui::TextWrapped("See reframework_faulty_files.txt for full list and details. Use external tool to find out what mod/patch is causing the issue.");

    bool changed = false;

    changed |= m_enabled->draw("Enable Faulty File Detector");
    changed |= m_max_recent_files->draw("Max Recent Files to Display");

    // Define reason display info
    struct ReasonInfo {
        const char* label;
        ImVec4 color;
    };
    
    static const std::map<FaultyReason, ReasonInfo> reason_info{
        {FaultyReason::Unknown, {"Unknown", ImVec4(0.8f, 0.8f, 0.8f, 1.0f)}},
        {FaultyReason::MissingFile, {"Missing File", ImVec4(1.0f, 1.0f, 0.0f, 1.0f)}},
        {FaultyReason::Invalid, {"Invalid File", ImVec4(1.0f, 0.5f, 0.0f, 1.0f)}},
        {FaultyReason::ShouldBeEncrypted, {"Should Be Encrypted", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)}},
    };

    if (ImGui::TreeNode("Show recent##ShowRecentFaultyFiles")) {
        // Display files organized by reason
        for (const auto& [reason, info] : reason_info) {
            auto it = m_recent_faulty_files_by_reason.find(reason);
            if (it == m_recent_faulty_files_by_reason.end() || it->second.empty()) {
                continue; // Skip if no files for this reason
            }

            const auto& files = it->second;
            const int count_to_display = std::min((int)files.size(), m_max_recent_files->value());
            const int total_for_reason = (int)std::count_if(
                m_faulty_files.begin(), m_faulty_files.end(),
                [&reason](const auto&) { return true; } // Simplified; in production you'd track per-reason totals
            );

            ImGui::PushStyleColor(ImGuiCol_Text, info.color);
            
            if (ImGui::TreeNode(std::format("{} ({})", info.label, files.size()).c_str())) {
                for (int i = 0; i < count_to_display; ++i) {
                    ImGui::TextWrapped("%s", utility::narrow(files[i]).c_str());
                }

                if (files.size() > (size_t)count_to_display) {
                    ImGui::TextWrapped("... and %zu more", files.size() - count_to_display);
                }

                ImGui::TreePop();
            }
            
            ImGui::PopStyleColor();
        }

        ImGui::TreePop();
    }

    if (changed) {
        g_framework->request_save_config();
    }
}

void FaultyFileDetector::early_init() {
    if (g_faulty_detector_instance == nullptr) {
        g_faulty_detector_instance = std::make_unique<FaultyFileDetector>();
    }

    g_faulty_detector_instance->initialize_impl();
}

std::shared_ptr<FaultyFileDetector>& FaultyFileDetector::get() {
    return g_faulty_detector_instance;
}