#pragma once

#include <reframework/API.hpp>

#pragma managed

namespace REFrameworkNET {
ref class ManagedObject;
ref class TypeDefinition;
ref class TypeInfo;

public ref struct ManagedSingleton {
    ManagedSingleton(ManagedObject^ instance, TypeDefinition^ type, TypeInfo^ typeInfo)
    {
        Instance = instance;
        Type = type;
        TypeInfo = typeInfo;
    }

    property ManagedObject^ Instance;
    property TypeDefinition^ Type;
    property TypeInfo^ TypeInfo;
};
}