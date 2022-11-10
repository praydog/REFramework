#include "ManagedObject.hpp"

namespace sdk {
void ManagedObject::add_ref() {
    ::utility::re_managed_object::add_ref(this);
}

void ManagedObject::release() {
    ::utility::re_managed_object::release(this);
}
}