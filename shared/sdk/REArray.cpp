#include "RETypeDB.hpp"
#include "RETypeDefinition.hpp"
#include "REArray.hpp"

namespace utility::re_array {
::sdk::RETypeDefinition* get_contained_type(::REArrayBase* container) {
    if (container == nullptr || container->containedType == nullptr) {
        return nullptr;
    }

#if TDB_VER > 49
    return (::sdk::RETypeDefinition*)container->containedType;
#else
    if (container->containedType->classInfo == nullptr) {
        return nullptr;
    }

    return (::sdk::RETypeDefinition*)container->containedType->classInfo;
#endif
}

uint32_t get_element_size(::REArrayBase* container) {
    if (!utility::re_array::has_inline_elements(container)) {
        return sizeof(void*);
    }

    const auto contained_type = get_contained_type(container);

    return utility::re_type::get_value_type_size(contained_type->get_type());
}

bool has_inline_elements(::REArrayBase* container) {
    const auto contained_type = get_contained_type(container);

    if (contained_type == nullptr) {
        return false;
    }

    return contained_type->get_vm_obj_type() == via::clr::VMObjType::ValType;
}
}