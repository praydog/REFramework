#pragma once

#include <cstdint>

class REThreadContext;
class RETypeDB;

namespace sdk {
    // AKA via.clr.VM
    class REGlobalContext {
    public:
        static REGlobalContext* get();

    public:
        REThreadContext* get_thread_context(int32_t unk = -1);
        RETypeDB* get_type_db();


    private:
        using ThreadContextFn = REThreadContext* (*)(REGlobalContext*, int32_t);
        static void update_pointers();

        static REGlobalContext** s_global_context;
        static ThreadContextFn s_get_thread_context;

        static int32_t s_type_db_offset;
    };

    REThreadContext* get_thread_context(int32_t unk = -1);
}