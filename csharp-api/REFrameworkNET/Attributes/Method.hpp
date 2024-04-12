#pragma once

#include "../TDB.hpp"

namespace REFrameworkNET::Attributes {
    // Attribute to mark a method as a method name
    [System::AttributeUsage(System::AttributeTargets::Method)]
    public ref class Method : public System::Attribute {
    public:
        Method(uint32_t methodIndex) {
            method = REFrameworkNET::TDB::Get()->GetMethod(methodIndex);
        }

        REFrameworkNET::Method^ GetMethod() {
            return method;
        }

    private:
        REFrameworkNET::Method^ method;
    };
}