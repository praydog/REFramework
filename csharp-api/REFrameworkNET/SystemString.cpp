#include "Field.hpp"
#include "API.hpp"
#include "VM.hpp"
#include "SystemString.hpp"

namespace REFrameworkNET {
SystemString::SystemString(::System::String^ str)
    : ManagedObject(VM::CreateString(str))
{

}

SystemString::SystemString(std::wstring_view str) 
    : ManagedObject(VM::CreateString(str))
{

}

SystemString::SystemString(std::string_view str)
    : ManagedObject(VM::CreateString(str))
{

}

::System::String^ SystemString::ToString() {
    auto strType = GetTypeDefinition();
    const auto firstCharField = strType->GetField("_firstChar");
    uint32_t offset = 0;

    if (firstCharField != nullptr) {
        offset = strType->GetField("_firstChar")->GetOffsetFromBase();
    } else {
        const auto fieldOffset = *(uint32_t*)(*(uintptr_t*)this->Ptr() - sizeof(void*));
        offset = fieldOffset + 4;
    }

    wchar_t* chars = (wchar_t*)((uintptr_t)this->Ptr() + offset);
    return gcnew System::String(chars);
}
}