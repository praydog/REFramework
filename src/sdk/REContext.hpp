class REThreadContext;

namespace sdk {
    struct REStaticTbl;
    class VM;
    REThreadContext* get_thread_context(int32_t unk = -1);
}


#pragma once

#include <cstdint>

#include "RETypeDB.hpp"

#ifdef RE7
#include "regenny/re7/via/clr/VM.hpp"
#endif

namespace sdk {
    struct REStaticTbl {
        uint8_t** elements;
        uint32_t size;
    };

    // AKA via.clr.VM
#ifdef RE7
    class VM : public regenny::via::clr::VM {
#else
    class VM {
#endif
    public:
        static VM* get();

    public:
        REThreadContext* get_thread_context(int32_t unk = -1);
        sdk::RETypeDB* get_type_db();
        REStaticTbl& get_static_tbl();

        uint8_t* get_static_tbl_for_type(uint32_t type_index);

    private:
        using ThreadContextFn = REThreadContext* (*)(VM*, int32_t);
        static void update_pointers();

        static VM** s_global_context;
        static ThreadContextFn s_get_thread_context;

        static int32_t s_static_tbl_offset;
        static int32_t s_type_db_offset;
    };
}