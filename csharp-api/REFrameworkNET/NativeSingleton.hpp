#pragma once

#include <reframework/API.hpp>

#pragma managed

namespace REFrameworkNET {
ref class NativeObject;
ref class TypeDefinition;
ref class TypeInfo;

public ref struct NativeSingleton {
    NativeSingleton(NativeObject^ instance, TypeInfo^ typeInfo)
    {
        Instance = instance;
        TypeInfo = typeInfo;
    }

    property NativeObject^ Instance;
    property TypeInfo^ TypeInfo;
};
}