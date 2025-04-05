#pragma once

#include <cstdint>

#include "TDBVer.hpp"
#include "RETypeDB.hpp"

namespace sdk {
    class VMContext;

    struct DelegateInvocation {
        using InvocationFn = void (*)(VMContext*, REManagedObject*);
        
        REManagedObject* object;
        InvocationFn func;
        sdk::REMethodDefinition* method;
    };
    
    struct Delegate : public REManagedObject {
        uint32_t num_methods{0};
        DelegateInvocation methods[1];

        void invoke();
    };
}