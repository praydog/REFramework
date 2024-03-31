#pragma once

#include <string_view>

#include "TypeDefinition.hpp"
#include "ManagedObject.hpp"

namespace REFrameworkNET {
public ref class SystemString : public ManagedObject {
public:
    SystemString(::REFrameworkManagedObjectHandle handle)
        : ManagedObject(handle)
    {
        const auto td = this->GetTypeDefinition();
        if (td == nullptr || td->GetVMObjType() != VMObjType::String) {
            throw gcnew System::ArgumentException("object is not a System.String");
        }
    }

    SystemString(ManagedObject^% object) : ManagedObject(object) {
        const auto td = object->GetTypeDefinition();
        if (td == nullptr || td->GetVMObjType() != VMObjType::String) {
            throw gcnew System::ArgumentException("object is not a System.String");
        }
    }

    SystemString(::System::String^ str);
    SystemString(std::wstring_view str);
    SystemString(std::string_view str);

    ::System::String^ ToString() override;

private:
};
}