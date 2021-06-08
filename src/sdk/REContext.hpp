#pragma once

#include <cstdint>

#include "RETypeDB.hpp"

class REThreadContext;

namespace sdk {
    struct REStaticTbl {
        uint8_t** elements;
        uint32_t size;
    };

    // AKA via.clr.VM
    class REGlobalContext {
    public:
        static REGlobalContext* get();

    public:
        REThreadContext* get_thread_context(int32_t unk = -1);
        sdk::RETypeDB* get_type_db();
        REStaticTbl& get_static_tbl();

        uint8_t* get_static_tbl_for_type(uint32_t type_index);

    private:
        using ThreadContextFn = REThreadContext* (*)(REGlobalContext*, int32_t);
        static void update_pointers();

        static REGlobalContext** s_global_context;
        static ThreadContextFn s_get_thread_context;

        static int32_t s_static_tbl_offset;
        static int32_t s_type_db_offset;
    };

    REThreadContext* get_thread_context(int32_t unk = -1);
}