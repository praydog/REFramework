#pragma once

#include "TypeDefinition.hpp"

namespace REFrameworkNET {
public ref struct MethodParameter {
    MethodParameter(const REFrameworkMethodParameter& p) { 
        Name = gcnew System::String(p.name);
        Type = gcnew REFrameworkNET::TypeDefinition(p.t);  
    }

    property System::String^ Name;
    property REFrameworkNET::TypeDefinition^ Type;
};
}