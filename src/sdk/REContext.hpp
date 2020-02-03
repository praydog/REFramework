#pragma once

#include <cstdint>

class REThreadContext;

namespace sdk {
    class REGlobalContext {
    public:
        static REGlobalContext* get();

    public:
        REThreadContext* getThreadContext(int32_t unk = -1);

    private:
        using ThreadContextFn = REThreadContext* (*)(REGlobalContext*, int32_t);
        static void updatePointers();

        static REGlobalContext** s_globalContext;
        static ThreadContextFn s_getThreadContext;
    };

    static REThreadContext* getThreadContext(int32_t unk = -1) {
        auto globalContext = REGlobalContext::get();

        if (globalContext == nullptr) {
            return nullptr;
        }

        return globalContext->getThreadContext(unk);
    }
}