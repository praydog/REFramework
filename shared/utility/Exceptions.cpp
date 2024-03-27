#include <windows.h>
#include <dbghelp.h>
#include <spdlog/spdlog.h>

#include <utility/Module.hpp>

#include "Exceptions.hpp"

namespace utility {
namespace exceptions{
static bool symbols_initialized = false;
static HANDLE process{};

void dump_callstack(EXCEPTION_POINTERS* exception) {
    const auto dbghelp = LoadLibraryA("dbghelp.dll");

    if (dbghelp == nullptr) {
        spdlog::error("Failed to load dbghelp.dll");
    }

    const auto sym_initialize = (decltype(&SymInitialize))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymInitialize") : nullptr);
    const auto sym_from_addr = (decltype(&SymFromAddr))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymFromAddr") : nullptr);
    const auto sym_set_options = (decltype(&SymSetOptions))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymSetOptions") : nullptr);
    const auto sym_get_line_from_addr64 = (decltype(&SymGetLineFromAddr))(dbghelp != nullptr ? GetProcAddress(dbghelp, "SymGetLineFromAddr64") : nullptr);
    
    const auto pid = GetCurrentProcessId();

    if (process == nullptr) {
        process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    }

    if (sym_set_options != nullptr) {
        sym_set_options(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    }

    if (!symbols_initialized && sym_initialize != nullptr) {
        symbols_initialized = sym_initialize(process, NULL, TRUE);
        if (!symbols_initialized) {
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

        if (sym_from_addr != nullptr && symbols_initialized) {
            symbol_found = sym_from_addr(process, (DWORD64)stack[i], 0, symbol);
        }

        std::string symbol_name = symbol_found ? symbol->Name : "Unknown symbol";

        if (sym_get_line_from_addr64 != nullptr && symbols_initialized && symbol_found) {
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
    //CloseHandle(process);
}
}
}