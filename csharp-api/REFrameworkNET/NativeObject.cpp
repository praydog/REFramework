#include "InvokeRet.hpp"
#include "Method.hpp"
#include "NativeObject.hpp"
#include "Proxy.hpp"

#include "API.hpp"

namespace REFrameworkNET {
generic <typename T>
T NativeObject::As() {
    return NativeProxy<T>::Create(this);
}
}