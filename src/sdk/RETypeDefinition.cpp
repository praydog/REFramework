#include <deque>
#include <mutex>
#include <shared_mutex>

#include "REFramework.hpp"

#include "RETypeDB.hpp"
#include "RETypeDefinition.hpp"

namespace sdk {
sdk::REMethodDefinition* RETypeDefinition::MethodIterator::begin() {
    if (m_parent->member_method == 0) {
        return nullptr;
    }

    auto tdb = g_framework->get_types()->get_type_db();

    return &(*tdb->methods)[m_parent->member_method];
}

sdk::REMethodDefinition* RETypeDefinition::MethodIterator::end() {
    if (m_parent->member_field == 0) {
        return nullptr;
    }

    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    const auto& impl = (*tdb->typesImpl)[m_parent->impl_index];
    const auto num_methods = impl.num_member_methods;
#else
    const auto num_methods = m_parent->num_member_method;
#endif

    return &(*tdb->methods)[m_parent->member_field + num_methods];
}

size_t RETypeDefinition::MethodIterator::size() {
    return ((uintptr_t)end() - (uintptr_t)begin()) / sizeof(sdk::REMethodDefinition);
}

sdk::REField* sdk::RETypeDefinition::FieldIterator::begin() {
    if (m_parent->member_field == 0) {
        return nullptr;
    }

    auto tdb = g_framework->get_types()->get_type_db();

    return &(*tdb->fields)[m_parent->member_field];
}

sdk::REField* sdk::RETypeDefinition::FieldIterator::end() {
    if (m_parent->member_field == 0) {
        return nullptr;
    }

    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    const auto& impl = (*tdb->typesImpl)[m_parent->impl_index];
    const auto num_fields = impl.num_member_fields;
#else
    const auto num_fields = m_parent->num_member_field;
#endif

    return &(*tdb->fields)[m_parent->member_field + num_fields];
}

size_t RETypeDefinition::FieldIterator::size() {
    return ((uintptr_t)end() - (uintptr_t)begin()) / sizeof(sdk::REField);
}

sdk::REProperty* RETypeDefinition::PropertyIterator::begin() {
    if (m_parent->member_prop == 0) {
        return nullptr;
    }

    auto tdb = g_framework->get_types()->get_type_db();

    return &(*tdb->properties)[m_parent->member_prop];
}

sdk::REProperty* RETypeDefinition::PropertyIterator::end() {
    if (m_parent->member_prop == 0) {
        return nullptr;
    }

    auto tdb = g_framework->get_types()->get_type_db();

    const auto num_prop = m_parent->num_member_prop;

    return &(*tdb->properties)[m_parent->member_prop + num_prop];
}

const char* RETypeDefinition::get_namespace() {
    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    auto& impl = (*tdb->typesImpl)[this->impl_index];

    const auto name_index = impl.namespace_offset;
#else
    const auto name_index = this->namespace_offset;
#endif

    return Address{tdb->stringPool}.get(name_index).as<const char*>();
}

const char* RETypeDefinition::get_name() {
    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    auto& impl = (*tdb->typesImpl)[this->impl_index];

    const auto name_index = impl.name_offset;
#else
    const auto name_index = this->name_offset;
#endif

    return Address{tdb->stringPool}.get(name_index).as<const char*>();
}

// because who knows where this is going to be called from.
static std::unordered_map<uint32_t, std::string> g_full_names{};
static std::shared_mutex g_full_name_mtx{};

std::string RETypeDefinition::get_full_name() {
    auto tdb = g_framework->get_types()->get_type_db();

    {
        std::shared_lock _{ g_full_name_mtx };

        if (g_full_names.find(this->index) != g_full_names.end()) {
            return g_full_names[this->index];
        }
    }

    std::deque<std::string> names{};
    std::string full_name{};

    if (this->declaring_typeid > 0 && this->declaring_typeid != this->index) {
        std::unordered_set<sdk::RETypeDefinition*> seen_classes{};

        for (auto owner = this; owner != nullptr; owner = owner->get_declaring_type()) {
            if (seen_classes.count(owner) > 0) {
                break;
            }

            names.push_front(owner->get_name());

            if (owner->get_declaring_type() == nullptr && !std::string{owner->get_namespace()}.empty()) {
                names.push_front(owner->get_namespace());
            }

            // uh.
            if (owner->get_declaring_type() == this) {
                break;
            }

            seen_classes.insert(owner);
        }
    } else {
        // namespace
        if (!std::string{this->get_namespace()}.empty()) {
            names.push_front(this->get_namespace());
        }

        // actual class name
        names.push_back(this->get_name());
    }

    for (auto f = 0; f < names.size(); ++f) {
        if (f > 0) {
            full_name += ".";
        }

        full_name += names[f];
    }

    // Set this here at this point in-case get_full_name runs into it
    {
        std::unique_lock _{g_full_name_mtx};
        g_full_names[this->index] = full_name;
    }

    if (this->generics > 0) {
        auto generics = (sdk::GenericListData*)&(*tdb->bytePool)[this->generics];

        if (generics->num > 0) {
            full_name += "<";

            for (uint32_t f = 0; f < generics->num; ++f) {
                auto gtypeid = generics->types[f];

                if (gtypeid > 0 && gtypeid < tdb->numTypes) {
                    auto& generic_type = (*tdb->types)[gtypeid];
                    full_name += generic_type.get_full_name();
                } else {
                    full_name += "";
                }

                if (generics->num > 1 && f < generics->num - 1) {
                    full_name += ",";
                }
            }

            full_name += ">";
        }
    }

    {
        std::unique_lock _{g_full_name_mtx};
        g_full_names[this->index] = full_name;
    }

    return full_name;
}

sdk::RETypeDefinition* RETypeDefinition::get_declaring_type() {
    auto tdb = g_framework->get_types()->get_type_db();

    if (this->declaring_typeid == 0 || this->declaring_typeid >= tdb->numTypes) {
        return nullptr;
    }

    return &(*tdb->types)[this->declaring_typeid];
}

sdk::RETypeDefinition* RETypeDefinition::get_parent_type() {
    auto tdb = g_framework->get_types()->get_type_db();

    if (this->parent_typeid == 0 || this->parent_typeid >= tdb->numTypes) {
        return nullptr;
    }

    return &(*tdb->types)[this->parent_typeid];
}

static std::shared_mutex g_field_mtx{};
static std::unordered_map<std::string, sdk::REField*> g_field_map{};

sdk::REField* RETypeDefinition::get_field(std::string_view name) {
    auto full_name = this->get_full_name() + "." + name.data();

    {
        std::shared_lock _{ g_field_mtx };

        if (g_field_map.find(full_name) != g_field_map.end()) {
            return g_field_map[full_name];
        }
    }

    for (auto& f : get_fields()) {
        auto field_name = std::string{};

        if (field_name == name) {
            std::unique_lock _{ g_field_mtx };

            g_field_map[full_name] = &f;
            return g_field_map[full_name];
        }
    }

    return nullptr;
}

static std::shared_mutex g_method_mtx{};
static std::unordered_map<std::string, REMethodDefinition*> g_method_map{};

sdk::REMethodDefinition* RETypeDefinition::get_method(std::string_view name) {
    auto full_name = this->get_full_name() + "." + name.data();

    {
        std::shared_lock _{g_method_mtx};

        if (g_method_map.find(full_name) != g_method_map.end()) {
            return g_method_map[full_name];
        }
    }

    for (auto& m  : get_methods()) {
        auto method_name = std::string{};

        if (method_name == name) {
            std::unique_lock _{g_method_mtx};

            g_method_map[full_name] = &m;
            return g_method_map[full_name];
        }
    }

    return nullptr;
}

int32_t RETypeDefinition::get_fieldptr_offset() {
    if (this->managed_vt == nullptr) {
        return 0;
    }

    return *(int32_t*)((uintptr_t)this->managed_vt - sizeof(void*));
}

via::clr::VMObjType RETypeDefinition::get_vm_obj_type() const {
    return (via::clr::VMObjType)this->object_type;
}
} // namespace sdk