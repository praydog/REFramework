#include "InvokeRet.hpp"
#include "Method.hpp"
#include "NativeObject.hpp"

namespace REFrameworkNET {
InvokeRet^ NativeObject::Invoke(System::String^ methodName, array<System::Object^>^ args) {
    auto t = this->GetTypeDefinition();
    
    if (t == nullptr) {
        return nullptr;
    }

    auto m = t->GetMethod(methodName);

    if (m == nullptr) {
        return nullptr;
    }

    return m->Invoke(this, args);
}
}