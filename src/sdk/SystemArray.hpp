namespace sdk {
struct SystemArray;
}

#pragma once

#include <vector>

#include "REManagedObject.hpp"

namespace sdk {
struct SystemArray : public ::REManagedObject {
    int32_t size();
    ::REManagedObject* get_element(int32_t index);
    void set_element(int32_t index, ::REManagedObject* value);
    std::vector<::REManagedObject*> get_elements();
};
}