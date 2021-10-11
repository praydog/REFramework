#include <shared_mutex>
#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "REFramework.hpp"
#include "ReClass.hpp"
#include "REContext.hpp"

namespace sdk {
    VM** VM::s_global_context{ nullptr };
    VM::ThreadContextFn VM::s_get_thread_context{ nullptr };
    int32_t VM::s_static_tbl_offset{ 0 };
    int32_t VM::s_type_db_offset{ 0 };

    sdk::VM* VM::get() {
        update_pointers();
        return *s_global_context;
    }

    REThreadContext* VM::get_thread_context(int32_t unk /*= -1*/) {
        update_pointers();

        return s_get_thread_context(this, unk);
    }

    sdk::RETypeDB* VM::get_type_db() {
        update_pointers();

        return *(sdk::RETypeDB**)((uintptr_t)this + s_type_db_offset);
    }

    REStaticTbl& VM::get_static_tbl() {
        update_pointers();

        return *(REStaticTbl*)((uintptr_t)this + s_static_tbl_offset);
    }

    uint8_t* VM::get_static_tbl_for_type(uint32_t type_index) {
#ifndef RE7
        auto& tbls = get_static_tbl();

        /*if (type_index >= tbls.size) {
            return nullptr;
        }*/

        return tbls.elements[type_index];
#else
        if (type_index >= this->num_types) {
            return nullptr;
        }

        return (uint8_t*)this->types[type_index].static_fields;
#endif
    }

    static std::shared_mutex s_mutex{};

    void VM::update_pointers() {
        {
            // Lock a shared lock for the s_mutex
            std::shared_lock lock(s_mutex);

            if (s_global_context != nullptr && s_get_thread_context != nullptr) {
                return;
            }
        }

        // Create a unique lock for the s_mutex as we get to the meat of the function
        std::unique_lock lock{ s_mutex };

        spdlog::info("[VM::update_pointers] Updating...");

        // Version 1
        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ? 48 89 C3");

        auto mod = g_framework->get_module().as<HMODULE>();
        auto start = (uintptr_t)mod;
        auto end = (uintptr_t)start + *utility::get_module_size(mod);

        std::unordered_map<uintptr_t, uint32_t> references{};

        struct CtxPattern {
            int32_t ctx_offset;
            int32_t get_thread_context_offset;
            std::string pattern;
        };

        auto patterns = std::vector<CtxPattern>{
            // Version 2 Dec 17th, 2019, first ptr is at game.exe+0x7095E08
            // Works RE2, RE3, RE8, DMC5
            { 3, 13, "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ?" },
             // Only seen in RE7
            { 3, 11, "48 8B 0D ? ? ? ? 83 CA FF E8 ? ? ? ?" },
        };


        std::optional<Address> ref{};
        const CtxPattern* context_pattern{nullptr};
        
        for (const auto& pattern : patterns) {
            ref = {};
            references.clear();

            const auto& pat = pattern.pattern;

            for (auto i = utility::scan(start, end - start, pat); i.has_value(); i = utility::scan(*i + 1, end - (*i + 1), pat)) {
                auto potential_ctx_ref = utility::calculate_absolute(*i + 3);

                references[potential_ctx_ref]++;

                // this is for sure the right one
                if (references[potential_ctx_ref] > 10) {
                    ref = *i;
                    context_pattern = &pattern;
                    break;
                }
            }

            if (ref) {
                break;
            }
        }

        if (!ref || *ref == nullptr) {
            spdlog::info("[VM::update_pointers] Unable to find ref.");
            return;
        }

        s_global_context = (decltype(s_global_context))utility::calculate_absolute(*ref + context_pattern->ctx_offset);
        s_get_thread_context = (decltype(s_get_thread_context))utility::calculate_absolute(*ref + context_pattern->get_thread_context_offset);

        for (auto i = 0; i < 0x20000; i += sizeof(void*)) {
            auto ptr = *(sdk::RETypeDB**)((uintptr_t)*s_global_context + i);

            if (ptr == nullptr || IsBadReadPtr(ptr, sizeof(void*)) || ((uintptr_t)ptr & (sizeof(void*) - 1)) != 0) {
                continue;
            }

            if (*(uint32_t*)ptr == *(uint32_t*)"TDB") {
                const auto version = *(uint32_t*)((uintptr_t)ptr + 4);

                s_type_db_offset = i;
                s_static_tbl_offset = s_type_db_offset - 0x30; // hope this holds true for the older gameS!!!!!!!!!!!!!!!!!!!
                spdlog::info("[VM::update_pointers] s_type_db_offset: {:x}", s_type_db_offset);
                spdlog::info("[VM::update_pointers] s_static_tbl_offset: {:x}", s_static_tbl_offset);
                spdlog::info("[VM::update_pointers] TDB Version: {}", version);
                break;
            }
        }

        spdlog::info("[VM::update_pointers] s_global_context: {:x}", (uintptr_t)s_global_context);
        spdlog::info("[VM::update_pointers] s_get_thread_context: {:x}", (uintptr_t)s_get_thread_context);
    }

    REThreadContext* get_thread_context(int32_t unk /*= -1*/) {
        auto global_context = VM::get();

        if (global_context == nullptr) {
            return nullptr;
        }

        return global_context->get_thread_context(unk);
    }
}


