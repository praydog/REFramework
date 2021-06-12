#include <shared_mutex>

#include "../REFramework.hpp"
#include "ReClass.hpp"

#include "RETypeDefinition.hpp"
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


#ifdef RE8
    auto class_info = (sdk::RETypeDefVersion69*)t->classInfo;

    return (*g_framework->get_types()->get_type_db()->typesImpl)[class_info->impl_index].field_size;
#else
    auto class_info = t->classInfo;
    return class_info->elementSize;
#endif
}

bool utility::re_type::is_clr_type(::REType* t) {
    return (t->flags & (int16_t)via::dti::decl::Script) != 0;
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

static std::shared_mutex insertion_mutex{};
static std::unordered_map<std::string, VariableDescriptor*> var_map{};

VariableDescriptor* utility::re_type::get_field_desc(::REType* t, std::string_view field) {
    if (t == nullptr) {
        return nullptr;
    }

    auto full_name = std::string{t->name} + "." + field.data();

    {
        std::shared_lock _{ insertion_mutex };

        if (var_map.find(full_name) != var_map.end()) {
            return var_map[full_name];
        }
    }

    for (; t != nullptr; t = t->super) {
        auto vars = get_variables(t);

        if (vars == nullptr) {
            continue;
        }

        for (auto i = 0; i < vars->num; ++i) {
            auto& var = vars->data->descriptors[i];

            if (var == nullptr || var->name == nullptr) {
                continue;
            }

            if (field == var->name) {
                std::unique_lock _{insertion_mutex};
                var_map[full_name] = var;
                return var;
            }
        }
    }

    return nullptr;
}

REVariableList* utility::re_type::get_variables(::REType* t) {
    if (t == nullptr || t->fields == nullptr || t->fields->variables == nullptr) {
        return nullptr;
    }

    auto vars = t->fields->variables;

    if (vars->data == nullptr || vars->num <= 0) {
        return nullptr;
    }

    return vars;
}

static std::shared_mutex method_insertion_mutex{};
static std::unordered_map<std::string, FunctionDescriptor*> method_map{};

FunctionDescriptor* utility::re_type::get_method_desc(::REType* t, std::string_view name) {
    if (t == nullptr) {
        return nullptr;
    }

    auto full_name = std::string{t->name} + "." + name.data();

    {
        std::shared_lock _{ method_insertion_mutex };

        if (method_map.find(full_name) != method_map.end()) {
            return method_map[full_name];
        }
    }

    for (; t != nullptr; t = t->super) {
        auto fields = t->fields;

        if (fields == nullptr || fields->methods == nullptr) {
            continue;
        }

        auto methods = fields->methods;

        for (auto i = 0; i < fields->num; ++i) {
            auto top = (*methods)[i];

            if (top == nullptr || *top == nullptr) {
                continue;
            }

            auto& holder = **top;

            if (holder.descriptor == nullptr || holder.descriptor->name == nullptr) {
                continue;
            }

            if (name == holder.descriptor->name) {
                std::unique_lock _{ method_insertion_mutex};
                method_map[full_name] = holder.descriptor;
                return holder.descriptor;
            }
        }
    }

    return nullptr;
}

