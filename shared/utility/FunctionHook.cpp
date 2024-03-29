#include <spdlog/spdlog.h>
#include <safetyhook/inline_hook.hpp>

#include "FunctionHook.hpp"

using namespace std;


FunctionHook::FunctionHook(Address target, Address destination)
    : m_target{ target },
    m_destination{ destination }
{
    spdlog::info("Attempting to hook {:p}->{:p}", target.ptr(), destination.ptr());

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
            return;
        }

        m_inline_hook = std::move(*expect);
    } catch (const std::exception& e) {
        spdlog::error("Failed to hook {:x}: {}", m_target, e.what());
    } catch (...) {
        spdlog::error("Failed to hook {:x}: unknown exception", m_target);
    }

    if (m_inline_hook) {
        spdlog::info("Hooked {:x}->{:x}", m_target, m_destination);
    } else {
        spdlog::error("Failed to hook {:x}", m_target);
    }
}

FunctionHook::~FunctionHook() {
}
