#include "InvokeRet.hpp"
#include "Method.hpp"
#include "NativeObject.hpp"
#include "Proxy.hpp"

#include "API.hpp"

namespace REFrameworkNET {
generic <typename T>
T NativeObject::As() {
    // Cannot cache nullptr objects
    /*if (this->GetAddress() == 0) {
        return NativeProxy<T>::Create(this);
    }

    if (auto existingProxy = this->GetProxy(T::typeid)) {
        return (T)existingProxy;
    }

    auto result = NativeProxy<T>::Create(this);

    this->AddProxy(T::typeid, (IProxy^)result);
    return result;*/

    // Not gonna bother with this for now. Just create a new proxy every time
    // Reason being, this is not a managed object, so we have no idea if the address
    // still points to the original object or not. It's better to just create a new one.
    return NativeProxy<T>::Create(this);
}
}