#include <shared_mutex>
#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

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

    sdk::RETypeDB* REGlobalContext::get_type_db() {
        update_pointers();

        return *(sdk::RETypeDB**)((uintptr_t)this + s_type_db_offset);
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

    static std::shared_mutex s_mutex{};

    void REGlobalContext::update_pointers() {
        {
            // Lock a shared lock for the s_mutex
            std::shared_lock lock(s_mutex);

            if (s_global_context != nullptr && s_get_thread_context != nullptr) {
                return;
            }
        }

        // Create a unique lock for the s_mutex as we get to the meat of the function
        std::unique_lock lock{ s_mutex };

        spdlog::info("[REGlobalContext::update_pointers] Updating...");

        // Version 1
        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ? 48 89 C3");

        auto mod = g_framework->get_module().as<HMODULE>();
        auto start = (uintptr_t)mod;
        auto end = (uintptr_t)start + *utility::get_module_size(mod);

        std::unordered_map<uintptr_t, uint32_t> references{};

        // Version 2 Dec 17th, 2019, first ptr is at game.exe+0x7095E08
        const auto pat = "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ?";
        std::optional<Address> ref{};

        for (auto i = utility::scan(start, end - start, pat); i.has_value(); i = utility::scan(*i + 1, end - (*i + 1), pat)) {
            auto potential_ctx_ref = utility::calculate_absolute(*i + 3);

            references[potential_ctx_ref]++;

            // this is for sure the right one
            if (references[potential_ctx_ref] > 10) {
                ref = *i;
                break;
            }
        }

        if (!ref || *ref == nullptr) {
            spdlog::info("[REGlobalContext::update_pointers] Unable to find ref.");
            return;
        }

        s_global_context = (decltype(s_global_context))utility::calculate_absolute(*ref + 3);
        s_get_thread_context = (decltype(s_get_thread_context))utility::calculate_absolute(*ref + 13);

        for (auto i = 0; i < 0x20000; i += sizeof(void*)) {
            auto ptr = *(sdk::RETypeDB**)((uintptr_t)*s_global_context + i);

            if (ptr == nullptr || IsBadReadPtr(ptr, sizeof(void*)) || ((uintptr_t)ptr & (sizeof(void*) - 1)) != 0) {
                continue;
            }

            if (*(uint32_t*)ptr == *(uint32_t*)"TDB") {
                s_type_db_offset = i;
                s_static_tbl_offset = s_type_db_offset - 0x30; // hope this holds true for the older gameS!!!!!!!!!!!!!!!!!!!
                spdlog::info("[REGlobalContext::update_pointers] s_type_db_offset: {:x}", s_type_db_offset);
                spdlog::info("[REGlobalContext::update_pointers] s_static_tbl_offset: {:x}", s_static_tbl_offset);
                break;
            }
        }

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


