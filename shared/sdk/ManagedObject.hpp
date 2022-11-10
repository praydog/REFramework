#pragma once

#include "REManagedObject.hpp"

namespace sdk {
class ManagedObject : public ::REManagedObject {
public:
    void add_ref();
    void release();
};
}