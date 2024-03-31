#include <spdlog/spdlog.h>
#include <MinHook.h>

#include <safetyhook/inline_hook.hpp>

#include "FunctionHook.hpp"

using namespace std;


FunctionHook::FunctionHook(Address target, Address destination)
    : m_target{ target },
    m_destination{ destination }
{
    spdlog::info("Attempting to hook {:p}->{:p}", target.ptr(), destination.ptr());

    // Create the hook. Call create afterwards to prevent race conditions accessing FunctionHook before it leaves its constructor.
    /*if (auto status = MH_CreateHook(target.as<LPVOID>(), destination.as<LPVOID>(), (LPVOID*)&m_original); status == MH_OK) {
        m_target = target;
        m_destination = destination;

        spdlog::info("Hook init successful {:p}->{:p}", target.ptr(), destination.ptr());
    }
    else {
        spdlog::error("Failed to hook {:p}: {}", target.ptr(), MH_StatusToString(status));
    }*/
}

FunctionHook::~FunctionHook() {
}

bool FunctionHook::create() {
    if (m_target == 0 || m_destination == 0 ) {
        spdlog::error("FunctionHook not initialized");
        return false;
    }

    /*if (auto status = MH_EnableHook((LPVOID)m_target); status != MH_OK) {
        m_original = 0;
        m_destination = 0;
        m_target = 0;

        spdlog::error("Failed to hook {:x}: {}", m_target, MH_StatusToString(status));
        return false;
    }*/

    try {
        auto expect = safetyhook::InlineHook::create(safetyhook::Allocator::global(), m_target, m_destination);

        if (!expect) {
            std::string error = "";
            switch (expect.error().type) {
                case safetyhook::InlineHook::Error::BAD_ALLOCATION:
                    error = "bad allocation";
                    break;
                case safetyhook::InlineHook::Error::FAILED_TO_DECODE_INSTRUCTION:
                    error = "failed to decode instruction";
                    break;
                case safetyhook::InlineHook::Error::SHORT_JUMP_IN_TRAMPOLINE:
                    error = "short jump in trampoline";
                    break;
                case safetyhook::InlineHook::Error::IP_RELATIVE_INSTRUCTION_OUT_OF_RANGE:
                    error = "IP relative instruction out of range";
                    break;
                case safetyhook::InlineHook::Error::UNSUPPORTED_INSTRUCTION_IN_TRAMPOLINE:
                    error = "unsupported instruction in trampoline";
                    break;
                case safetyhook::InlineHook::Error::FAILED_TO_UNPROTECT:
                    error = "failed to unprotect memory";
                    break;
                case safetyhook::InlineHook::Error::NOT_ENOUGH_SPACE:
                    error = "not enough space";
                    break;
                default:
                    error = std::format("unknown error {}", (int32_t)expect.error().type);
                    break;
            };

            spdlog::error("Failed to hook {:x}: {}", m_target, error);
            return false;
        }

        m_inline_hook = std::move(*expect);
    } catch (const std::exception& e) {
        spdlog::error("Failed to hook {:x}: {}", m_target, e.what());
        return false;
    } catch (...) {
        spdlog::error("Failed to hook {:x}: unknown exception", m_target);
        return false;
    }

    if (m_inline_hook) {
        spdlog::info("Hooked {:x}->{:x}", m_target, m_destination);
    } else {
        spdlog::error("Failed to hook {:x}", m_target);
        return false;
    }

    return true;
}