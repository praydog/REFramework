#include <windows.h>
#include <DbgHelp.h>
#include <ShlObj.h>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "utility/Module.hpp"
#include "utility/Scan.hpp"
#include "utility/Patch.hpp"

#include "REFramework.hpp"
#include "ExceptionHandler.hpp"

void dump_call_stack(EXCEPTION_POINTERS* exception) {
    const auto dbghelp = LoadLibraryA("dbghelp.dll");

    if (dbghelp == nullptr) {
        spdlog::error("Failed to load dbghelp.dll");
    }

    const auto sym_initialize = (decltype(&SymInitialize))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymInitialize") : nullptr);
    const auto sym_from_addr = (decltype(&SymFromAddr))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymFromAddr") : nullptr);
    const auto sym_set_options = (decltype(&SymSetOptions))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymSetOptions") : nullptr);
    const auto sym_get_line_from_addr64 = (decltype(&SymGetLineFromAddr))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymGetLineFromAddr64") : nullptr);
    
    const auto pid = GetCurrentProcessId();
    const auto process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (sym_set_options != nullptr) {
        sym_set_options(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    }

    bool initialized = false;

    if (sym_initialize != nullptr) {
        initialized = sym_initialize(process, NULL, TRUE);
        if (!initialized) {
            spdlog::error("Failed to initialize symbol handler");
        }
    }

    constexpr auto max_stack_depth = 100;
    uintptr_t stack[max_stack_depth]{};

    const auto depth = RtlCaptureStackBackTrace(0, max_stack_depth, (void**)stack, nullptr);

    char symbol_data[sizeof(SYMBOL_INFO) + (256 * sizeof(char))]{};
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)symbol_data;

    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    spdlog::error("Call stack:");
    std::string stack_message{"\n"};
    for (auto i = 0; i < depth; ++i) {
        bool symbol_found = false;
        const auto module_within = utility::get_module_within(stack[i]);

        if (sym_from_addr != nullptr && initialized) {
            symbol_found = sym_from_addr(process, (DWORD64)stack[i], 0, symbol);
        }

        std::string symbol_name = symbol_found ? symbol->Name : "Unknown symbol";

        if (sym_get_line_from_addr64 != nullptr && initialized && symbol_found) {
            DWORD displacement = 0;
            IMAGEHLP_LINE64 line{};

            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

            if (sym_get_line_from_addr64(process, (DWORD64)stack[i], &displacement, &line)) {
                symbol_name += " (";
                symbol_name += line.FileName;
                symbol_name += ":";
                symbol_name += std::to_string(line.LineNumber);
                symbol_name += ")";
            }
        }
        
        if (module_within) {
            const auto module_path = utility::get_module_path(*module_within);
            const auto relative = stack[i] - (uintptr_t)*module_within;

            if (module_path) {
                stack_message += fmt::format(" {}\n  {} + 0x{:x}\n\n", symbol_name, *module_path, relative);
                continue;
            }

            stack_message += fmt::format(" {}\n  0x{:x} + 0x{:x}\n\n", symbol_name, (uintptr_t)*module_within, relative);
            continue;
        }

        stack_message += fmt::format(" {}\n  0x{:x}\n\n", symbol_name, stack[i]);
    }

    spdlog::error(stack_message);
    CloseHandle(process);
}

LONG WINAPI reframework::global_exception_handler(struct _EXCEPTION_POINTERS* ei) {
    static std::recursive_mutex mtx{};
    std::scoped_lock _{ mtx };

    spdlog::flush_on(spdlog::level::err);

    spdlog::error("Exception occurred: {:x}", ei->ExceptionRecord->ExceptionCode);
    spdlog::error("RIP: {:x}", ei->ContextRecord->Rip);
    spdlog::error("RSP: {:x}", ei->ContextRecord->Rsp);
    spdlog::error("RCX: {:x}", ei->ContextRecord->Rcx);
    spdlog::error("RDX: {:x}", ei->ContextRecord->Rdx);
    spdlog::error("R8: {:x}", ei->ContextRecord->R8);
    spdlog::error("R9: {:x}", ei->ContextRecord->R9);
    spdlog::error("R10: {:x}", ei->ContextRecord->R10);
    spdlog::error("R11: {:x}", ei->ContextRecord->R11);
    spdlog::error("R12: {:x}", ei->ContextRecord->R12);
    spdlog::error("R13: {:x}", ei->ContextRecord->R13);
    spdlog::error("R14: {:x}", ei->ContextRecord->R14);
    spdlog::error("R15: {:x}", ei->ContextRecord->R15);
    spdlog::error("RAX: {:x}", ei->ContextRecord->Rax);
    spdlog::error("RBX: {:x}", ei->ContextRecord->Rbx);
    spdlog::error("RBP: {:x}", ei->ContextRecord->Rbp);
    spdlog::error("RSI: {:x}", ei->ContextRecord->Rsi);
    spdlog::error("RDI: {:x}", ei->ContextRecord->Rdi);
    spdlog::error("EFLAGS: {:x}", ei->ContextRecord->EFlags);
    spdlog::error("CS: {:x}", ei->ContextRecord->SegCs);
    spdlog::error("DS: {:x}", ei->ContextRecord->SegDs);
    spdlog::error("ES: {:x}", ei->ContextRecord->SegEs);
    spdlog::error("FS: {:x}", ei->ContextRecord->SegFs);
    spdlog::error("GS: {:x}", ei->ContextRecord->SegGs);
    spdlog::error("SS: {:x}", ei->ContextRecord->SegSs);

    dump_call_stack(ei);

    const auto module_within = utility::get_module_within(ei->ContextRecord->Rip);

    if (module_within) {
#ifdef RE8
        // funny way to fix a crash.
        if (*module_within == utility::get_executable() && (uint32_t)ei->ContextRecord->Rcx == 0xFFFFFFFF) {
            spdlog::info("Attempting to fix RE8 overlay draw crash...");

            if (utility::scan(ei->ContextRecord->Rip, 4, "48 8B 9C CE")) {
                const auto offset = *(uint32_t*)(ei->ContextRecord->Rip + 4);
                std::vector<uint8_t> patch_bytes{ 0x48, 0x8B, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x90 };
                *(uint32_t*)(patch_bytes.data() + 3) = offset;
                std::vector<int16_t> patch_int16_bytes{};

                for (auto& patch_byte : patch_bytes) {
                    patch_int16_bytes.push_back(patch_byte);
                }

                static auto patch = Patch::create(ei->ContextRecord->Rip, patch_int16_bytes);
                spdlog::info("Successfully patched RE8 overlay draw crash.");
                return EXCEPTION_CONTINUE_EXECUTION;
            } else {
                spdlog::info("Instructions did not match RE8 overlay draw crash.");
            }
        }
#endif

        const auto module_path = utility::get_module_path(*module_within);

        if (module_path) {
            spdlog::error("Module: {:x} {}", (uintptr_t)*module_within, *module_path);
        } else {
            spdlog::error("Module: Unknown");
        }
    } else {
        spdlog::error("Module: Unknown");
    }

    auto dbghelp = LoadLibrary("dbghelp.dll");

    if (dbghelp) {
        const auto final_path = REFramework::get_persistent_dir("reframework_crash.dmp").string();

        spdlog::error("Attempting to write dump to {}", final_path);

        auto f = CreateFile(final_path.c_str(), 
            GENERIC_WRITE, 
            FILE_SHARE_WRITE, 
            nullptr, 
            CREATE_ALWAYS, 
            FILE_ATTRIBUTE_NORMAL, 
            nullptr
        );

        if (!f || f == INVALID_HANDLE_VALUE) {
            spdlog::error("Exception occurred, but could not create dump file");
            return EXCEPTION_CONTINUE_SEARCH;
        }

        MINIDUMP_EXCEPTION_INFORMATION ei_info{
            GetCurrentThreadId(),
            ei,
            FALSE
        };

        auto minidump_write_dump = (decltype(MiniDumpWriteDump)*)GetProcAddress(dbghelp, "MiniDumpWriteDump");

        minidump_write_dump(GetCurrentProcess(), 
            GetCurrentProcessId(),
            f,
            MINIDUMP_TYPE::MiniDumpNormal, 
            &ei_info, 
            nullptr, 
            nullptr
        );

        CloseHandle(f);
    } else {
        spdlog::error("Exception occurred, but could not load dbghelp.dll");
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

void reframework::setup_exception_handler() {
    SetUnhandledExceptionFilter(global_exception_handler);
}
