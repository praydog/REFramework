#pragma once

#include <string_view>

namespace REFrameworkNET {
ref class ManagedObject;
ref class SystemString;

public ref class VM {
public:
    static SystemString^ CreateString(::System::String^ str);
    static SystemString^ CreateString(std::wstring_view str);
    static SystemString^ CreateString(std::string_view str);

private:
};
}