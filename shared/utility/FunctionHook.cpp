#include <spdlog/spdlog.h>

#include <safetyhook/inline_hook.hpp>

#include "FunctionHook.hpp"

using namespace std;


FunctionHook::FunctionHook(Address target, Address destination)
    : m_target{ target },
    m_destination{ destination }
{
    spdlog::info("Attempting to hook {:p}->{:p}", target.ptr(), destination.ptr());
}

FunctionHook::~FunctionHook() {
}

bool FunctionHook::create() {
    std::unique_lock _{ m_initialization_mutex };

    if (m_target == 0 || m_destination == 0 ) {
        spdlog::error("FunctionHook not initialized");
        return false;
    }

    if (is_valid()) {
        return true;
    }

    try {
        m_inline_hook = safetyhook::InlineHook::create(m_target, m_destination);

        if (!m_inline_hook) {
            std::string error = "";
            switch (m_inline_hook.error().type) {
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
                    error = std::format("unknown error {}", (int32_t)m_inline_hook.error().type);
                    break;
            };

            spdlog::error("Failed to hook {:x}: {}", m_target, error);
            return false;
        }
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