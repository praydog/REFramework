#pragma once

#include <cstdint>

class REThreadContext;

namespace sdk {
    class REGlobalContext {
    public:
        static REGlobalContext* get();

    public:
        REThreadContext* get_thread_context(int32_t unk = -1);

    private:
        using ThreadContextFn = REThreadContext* (*)(REGlobalContext*, int32_t);
        static void update_pointers();

        static REGlobalContext** s_global_context;
        static ThreadContextFn s_get_thread_context;
    };

    REThreadContext* get_thread_context(int32_t unk = -1);
}