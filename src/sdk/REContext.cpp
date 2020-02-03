#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"

#include "REFramework.hpp"
#include "REContext.hpp"

namespace sdk {
    REGlobalContext** REGlobalContext::s_globalContext{ nullptr };
    REGlobalContext::ThreadContextFn REGlobalContext::s_getThreadContext{ nullptr };

    sdk::REGlobalContext* REGlobalContext::get() {
        updatePointers();
        return *s_globalContext;
    }

    REThreadContext* REGlobalContext::getThreadContext(int32_t unk /*= -1*/) {
        updatePointers();

        return s_getThreadContext(this, unk);
    }

    void REGlobalContext::updatePointers() {
        if (s_globalContext != nullptr && s_getThreadContext != nullptr) {
            return;
        }

        // Version 1
        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ? 48 89 C3");

        // Version 2 Dec 17th, 2019, first ptr is at game.exe+0x7095E08
        auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ?");
            
        if (!ref) {
            spdlog::info("[REGlobalContext::updatePointers] Unable to find ref.");
            return;
        }

        s_globalContext = (decltype(s_globalContext))utility::calculateAbsolute(*ref + 3);
        s_getThreadContext = (decltype(s_getThreadContext))utility::calculateAbsolute(*ref + 13);

        spdlog::info("[REGlobalContext::updatePointers] s_globalContext: {:x}", (uintptr_t)s_globalContext);
        spdlog::info("[REGlobalContext::updatePointers] s_getThreadContext: {:x}", (uintptr_t)s_getThreadContext);
    }
}


