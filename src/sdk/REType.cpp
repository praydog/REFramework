#include "../REFramework.hpp"
#include "ReClass.hpp"

#include "REType.hpp"

uint32_t utility::re_type::get_vm_type(::REType* t) {
    if (t == nullptr || t->classInfo == nullptr) {
        return (uint32_t)via::clr::VMObjType::NULL_;
    }

#ifndef RE8
    return (uint32_t)t->classInfo->objectType;
#else
    return (uint32_t)(t->classInfo->objectFlags >> 5);
#endif
}

uint32_t utility::re_type::get_value_type_size(::REType* t) {
    if (t == nullptr || t->classInfo == nullptr) {
        return 0;
    }

    if (get_vm_type(t) != (uint32_t)via::clr::VMObjType::ValType) {
        return t->size;
    }

    auto class_info = t->classInfo;

#ifdef RE8
    return (*g_framework->get_types()->get_type_db()->typesImpl)[(class_info->elementBitField >> 4) & ((1 << 18) - 1)].fieldSize;
#else
    return class_info->elementSize;
#endif
}

bool utility::re_type::is_singleton(::REType* t) {
    return (t->flags & (uint16_t)via::dti::decl::Singleton) != 0;
}

void* utility::re_type::get_singleton_instance(::REType* t) {
    if (!is_singleton(t)) {
        return nullptr;
    }

    using SingletonFunc = void (*)(::REType*, void**, void*);

    auto f = (*(SingletonFunc**)t)[1];

    void* out = nullptr;
    f(t, &out, nullptr);

    return out;
}
