#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"

#include "REFramework.hpp"
#include "REContext.hpp"

namespace sdk {
    REGlobalContext** REGlobalContext::s_global_context{ nullptr };
    REGlobalContext::ThreadContextFn REGlobalContext::s_get_thread_context{ nullptr };

    sdk::REGlobalContext* REGlobalContext::get() {
        update_pointers();
        return *s_global_context;
    }

    REThreadContext* REGlobalContext::get_thread_context(int32_t unk /*= -1*/) {
        update_pointers();

        return s_get_thread_context(this, unk);
    }

    void REGlobalContext::update_pointers() {
        if (s_global_context != nullptr && s_get_thread_context != nullptr) {
            return;
        }

        // Version 1
        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ? 48 89 C3");

        // Version 2 Dec 17th, 2019, first ptr is at game.exe+0x7095E08
        auto ref = utility::scan(g_framework->get_module().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ?");
            
        if (!ref) {
            spdlog::info("[REGlobalContext::updatePointers] Unable to find ref.");
            return;
        }

        s_global_context = (decltype(s_global_context))utility::calculate_absolute(*ref + 3);
        s_get_thread_context = (decltype(s_get_thread_context))utility::calculate_absolute(*ref + 13);

        spdlog::info("[REGlobalContext::updatePointers] s_globalContext: {:x}", (uintptr_t)s_global_context);
        spdlog::info("[REGlobalContext::updatePointers] s_getThreadContext: {:x}", (uintptr_t)s_get_thread_context);
    }

    REThreadContext* get_thread_context(int32_t unk /*= -1*/) {
        auto global_context = REGlobalContext::get();

        if (global_context == nullptr) {
            return nullptr;
        }

        return global_context->get_thread_context(unk);
    }
}


