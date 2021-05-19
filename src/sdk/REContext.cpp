#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"

#include "REFramework.hpp"
#include "ReClass.hpp"
#include "REContext.hpp"

namespace sdk {
    REGlobalContext** REGlobalContext::s_global_context{ nullptr };
    REGlobalContext::ThreadContextFn REGlobalContext::s_get_thread_context{ nullptr };
    int32_t REGlobalContext::s_static_tbl_offset{ 0 };
    int32_t REGlobalContext::s_type_db_offset{ 0 };

    sdk::REGlobalContext* REGlobalContext::get() {
        update_pointers();
        return *s_global_context;
    }

    REThreadContext* REGlobalContext::get_thread_context(int32_t unk /*= -1*/) {
        update_pointers();

        return s_get_thread_context(this, unk);
    }

    RETypeDB* REGlobalContext::get_type_db() {
        update_pointers();

        return *(RETypeDB**)((uintptr_t)this + s_type_db_offset);
    }

    REStaticTbl& REGlobalContext::get_static_tbl() {
        update_pointers();

        return *(REStaticTbl*)((uintptr_t)this + s_static_tbl_offset);
    }

    uint8_t* REGlobalContext::get_static_tbl_for_type(uint32_t type_index) {
        auto& tbls = get_static_tbl();

        /*if (type_index >= tbls.size) {
            return nullptr;
        }*/

        return tbls.elements[type_index];
    }

    void REGlobalContext::update_pointers() {
        if (s_global_context != nullptr && s_get_thread_context != nullptr) {
            return;
        }

        spdlog::info("[REGlobalContext::update_pointers] Updating...");

        // Version 1
        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ? 48 89 C3");

        // Version 2 Dec 17th, 2019, first ptr is at game.exe+0x7095E08
        auto ref = utility::scan(g_framework->get_module().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ?");
            
        if (!ref) {
            spdlog::info("[REGlobalContext::update_pointers] Unable to find ref.");
            return;
        }

        s_global_context = (decltype(s_global_context))utility::calculate_absolute(*ref + 3);
        s_get_thread_context = (decltype(s_get_thread_context))utility::calculate_absolute(*ref + 13);

#ifdef RE8
        for (auto i = 0; i < 0x20000; i += sizeof(void*)) {
            auto ptr = *(RETypeDB**)((uintptr_t)*s_global_context + i);

            if (ptr == nullptr || IsBadReadPtr(ptr, sizeof(void*)) || ((uintptr_t)ptr & (sizeof(void*) - 1)) != 0) {
                continue;
            }

            if (ptr->magic == *(uint32_t*)"TDB") {
                s_type_db_offset = i;
                s_static_tbl_offset = s_type_db_offset - 0x30; // hope this holds true for the older gameS!!!!!!!!!!!!!!!!!!!
                spdlog::info("[REGlobalContext::update_pointers] s_type_db_offset: {:x}", s_type_db_offset);
                spdlog::info("[REGlobalContext::update_pointers] s_static_tbl_offset: {:x}", s_static_tbl_offset);
                break;
            }
        }
#endif

        spdlog::info("[REGlobalContext::update_pointers] s_global_context: {:x}", (uintptr_t)s_global_context);
        spdlog::info("[REGlobalContext::update_pointers] s_get_thread_context: {:x}", (uintptr_t)s_get_thread_context);
    }

    REThreadContext* get_thread_context(int32_t unk /*= -1*/) {
        auto global_context = REGlobalContext::get();

        if (global_context == nullptr) {
            return nullptr;
        }

        return global_context->get_thread_context(unk);
    }
}


