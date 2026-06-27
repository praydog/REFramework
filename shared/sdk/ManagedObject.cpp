#include "ManagedObject.hpp"

namespace sdk {
void ManagedObject::add_ref() {
    ::REManagedObject::add_ref();
}

void ManagedObject::release() {
    ::REManagedObject::release();
}
}
