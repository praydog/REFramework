#include <windows.h>
#include <DbgHelp.h>
#include <ShlObj.h>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "utility/Module.hpp"
#include "utility/Scan.hpp"
#include "utility/Patch.hpp"

#include "utility/Exceptions.hpp"

#include "REFramework.hpp"
#include "ExceptionHandler.hpp"

LONG WINAPI reframework::global_exception_handler(struct _EXCEPTION_POINTERS* ei) {
    static std::recursive_mutex mtx{};
    std::scoped_lock _{ mtx };

    spdlog::flush_on(spdlog::level::err);

    if (_ReturnAddress() == nullptr) {
        spdlog::warn("SUSPICIOUS EXCEPTION: _ReturnAddress() == nullptr");
    }

    spdlog::error("Exception occurred: {:x}", ei->ExceptionRecord->ExceptionCode);
    
    spdlog::error("RIP (exception record): {:x}", (uintptr_t)ei->ExceptionRecord->ExceptionAddress);
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

    utility::exceptions::dump_callstack(ei);

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
