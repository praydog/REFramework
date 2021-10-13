#include <deque>
#include <mutex>
#include <shared_mutex>

#include "REFramework.hpp"

#include "RETypeDB.hpp"
#include "RETypeDefinition.hpp"

namespace sdk {
struct RETypeDefinition;

sdk::REMethodDefinition* RETypeDefinition::MethodIterator::begin() const {
    if (m_parent->member_method == 0) {
        return nullptr;
    }

    auto tdb = RETypeDB::get();

    return &(*tdb->methods)[m_parent->member_method];
}

sdk::REMethodDefinition* RETypeDefinition::MethodIterator::end()  const {
    if (m_parent->member_method == 0) {
        return nullptr;
    }

    auto tdb = RETypeDB::get();

#if TDB_VER >= 69
    const auto& impl = (*tdb->typesImpl)[m_parent->impl_index];
    const auto num_methods = impl.num_member_methods;
#else
    const auto num_methods = m_parent->num_member_method;
#endif

    return &(*tdb->methods)[m_parent->member_method + num_methods];
}

size_t RETypeDefinition::MethodIterator::size() const {
    return ((uintptr_t)end() - (uintptr_t)begin()) / sizeof(sdk::REMethodDefinition);
}

sdk::REField* sdk::RETypeDefinition::FieldIterator::REFieldIterator::operator*() const {
//#ifndef RE7
    if (m_parent->member_field == 0) {
        return nullptr;
    }

    auto tdb = RETypeDB::get();

    return &(*tdb->fields)[m_parent->member_field + m_index];
/*#else
    if (m_parent->member_field_start == 0) {
        return nullptr;
    }

    auto tdb = RETypeDB::get();
    const auto field_indices = tdb->get_data<uint32_t>(m_parent->member_field_start);

    const auto index = field_indices[m_index];

    if (index == 0) {
        return nullptr;
    }

    return &(*tdb->fields)[index];
#endif*/
}

size_t sdk::RETypeDefinition::FieldIterator::size() const {
#if TDB_VER >= 69
        auto tdb = sdk::RETypeDB::get();
        const auto& impl = (*tdb->typesImpl)[m_parent->impl_index];
        const auto num_fields = impl.num_member_fields;
#else
        const auto num_fields = m_parent->num_member_field;
#endif

        return num_fields;
    }

sdk::REProperty* RETypeDefinition::PropertyIterator::begin() const {
    if (m_parent->member_prop == 0) {
        return nullptr;
    }

    auto tdb = RETypeDB::get();

    return &(*tdb->properties)[m_parent->member_prop];
}

sdk::REProperty* RETypeDefinition::PropertyIterator::end()const {
    if (m_parent->member_prop == 0) {
        return nullptr;
    }

    auto tdb = RETypeDB::get();

    const auto num_prop = m_parent->num_member_prop;

    return &(*tdb->properties)[m_parent->member_prop + num_prop];
}

const char* RETypeDefinition::get_namespace() const {
    auto tdb = RETypeDB::get();

#if TDB_VER >= 69
    auto& impl = (*tdb->typesImpl)[this->impl_index];

    const auto name_index = impl.namespace_offset;
#else
    const auto name_index = this->namespace_offset;
#endif

    return tdb->get_string(name_index);
}

const char* RETypeDefinition::get_name() const {
    auto tdb = RETypeDB::get();

#if TDB_VER >= 69
    auto& impl = (*tdb->typesImpl)[this->impl_index];

    const auto name_index = impl.name_offset;
#else
    const auto name_index = this->name_offset;
#endif

    return tdb->get_string(name_index);
}

// because who knows where this is going to be called from.
static std::unordered_map<uint32_t, std::string> g_full_names{};
static std::shared_mutex g_full_name_mtx{};

std::string RETypeDefinition::get_full_name() const {
    auto tdb = RETypeDB::get();

    {
        std::shared_lock _{ g_full_name_mtx };

        if (g_full_names.find(this->get_index()) != g_full_names.end()) {
            return g_full_names[this->get_index()];
        }
    }

    std::deque<std::string> names{};
    std::string full_name{};

    if (this->declaring_typeid > 0 && this->declaring_typeid != this->get_index()) {
        std::unordered_set<const sdk::RETypeDefinition*> seen_classes{};

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
        g_full_names[this->get_index()] = full_name;
    }

#ifndef RE7
    if (this->generics > 0) {
        auto generics = tdb->get_data<sdk::GenericListData>(this->generics);

        if (generics->num > 0) {
            full_name += "<";

            for (uint32_t f = 0; f < generics->num; ++f) {
                auto gtypeid = generics->types[f];

                if (gtypeid > 0 && gtypeid < tdb->numTypes) {
                    auto& generic_type = *tdb->get_type(gtypeid);
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
#else
    //full_name += "<not implemented>";
#endif

    {
        std::unique_lock _{g_full_name_mtx};
        g_full_names[this->get_index()] = full_name;
    }

    return full_name;
}

sdk::RETypeDefinition* RETypeDefinition::get_declaring_type() const {
    auto tdb = RETypeDB::get();

    if (this->declaring_typeid == 0 || this->declaring_typeid >= tdb->numTypes) {
        return nullptr;
    }

    return tdb->get_type(this->declaring_typeid);
}

sdk::RETypeDefinition* RETypeDefinition::get_parent_type() const {
    auto tdb = RETypeDB::get();

    if (this->parent_typeid == 0 || this->parent_typeid >= tdb->numTypes) {
        return nullptr;
    }

    return tdb->get_type(this->parent_typeid);
}

static std::shared_mutex g_field_mtx{};
static std::unordered_map<std::string, sdk::REField*> g_field_map{};

sdk::REField* RETypeDefinition::get_field(std::string_view name) const {
    auto full_name = std::to_string(this->get_index()) + "." + name.data();

    {
        std::shared_lock _{ g_field_mtx };

        if (g_field_map.find(full_name) != g_field_map.end()) {
            return g_field_map[full_name];
        }
    }

    for (auto super = this; super != nullptr; super = super->get_parent_type()) {
        for (auto f : super->get_fields()) {
            if (name == f->get_name()) {
                std::unique_lock _{ g_field_mtx };

                g_field_map[full_name] = f;
                return g_field_map[full_name];
            }
        }
    }

    return g_field_map[full_name];
}

static std::shared_mutex g_method_mtx{};
static std::unordered_map<std::string, REMethodDefinition*> g_method_map{};

sdk::REMethodDefinition* RETypeDefinition::get_method(std::string_view name) const {
    // originally this used this->get_full_name() + "." + name.data()
    // but that doesn't work for generic types if we haven't yet mapped out
    // how generic (instantiated) types work for the game we're working with
    // and this is probably faster anyways
    auto full_name = std::to_string(this->get_index()) + "." + name.data();

    {
        std::shared_lock _{g_method_mtx};

        if (g_method_map.find(full_name) != g_method_map.end()) {
            return g_method_map[full_name];
        }
    }
    
    for (auto super = this; super != nullptr; super = super->get_parent_type()) {
        for (auto& m : super->get_methods()) {
            if (name == m.get_name()) {
                std::unique_lock _{g_method_mtx};

                g_method_map[full_name] = &m;
                return g_method_map[full_name];
            }
        }
    }

    return g_method_map[full_name];
}

uint32_t RETypeDefinition::get_index() const {
#ifndef RE7
    return this->index;
#else
    const auto tdb = RETypeDB::get();

    return (uint32_t)(((uintptr_t)this - (uintptr_t)tdb->types) / sizeof(sdk::RETypeDefinition));
#endif
}

int32_t RETypeDefinition::get_fieldptr_offset() const {
#ifndef RE7
    if (this->managed_vt == nullptr) {
        return 0;
    }

    return *(int32_t*)((uintptr_t)this->managed_vt - sizeof(void*));
#else
    auto vm = sdk::VM::get();
    const auto& vm_type = vm->types[this->get_index()];

    return vm_type.fieldptr_offset;
#endif
}

bool RETypeDefinition::has_fieldptr_offset() const {
#ifndef RE7
    return this->managed_vt != nullptr;
#else
    return true;
#endif
}

via::clr::VMObjType RETypeDefinition::get_vm_obj_type() const {
    return (via::clr::VMObjType)this->object_type;
}

uint32_t RETypeDefinition::get_crc_hash() const {
#ifndef RE7
    const auto t = get_type();
    return t != nullptr ? t->typeCRC : this->type_crc;
#else
    const auto t = (regenny::via::typeinfo::TypeInfo*)get_type();

    if (t == nullptr) {
        return 0;
    }

    return t->crc;
#endif
}

uint32_t RETypeDefinition::get_fqn_hash() const {
#ifndef RE7
    return this->fqn_hash;
#else
    auto t = (regenny::via::typeinfo::TypeInfo*)get_type();

    if (t == nullptr) {
        return 0;
    }

    return t->fqn_hash;
#endif
}

uint32_t RETypeDefinition::get_size() const {
#ifndef RE7
    return this->size;
#else
    auto t = (regenny::via::typeinfo::TypeInfo*)get_type();

    if (t == nullptr) {
        return 0;
    }

    return t->size;
#endif
}

uint32_t RETypeDefinition::get_valuetype_size() const {
#if TDB_VER >= 69
    auto tdb = RETypeDB::get();
    auto impl_id = this->impl_index;

    if (impl_id == 0) {
        return 0;
    }

    return (*tdb->typesImpl)[impl_id].field_size;
#else
    return ((REClassInfo*)this)->elementSize;
#endif
}

::REType* RETypeDefinition::get_type() const {
#ifndef RE7
    return this->type;
#else
    auto vm = sdk::VM::get();
    const auto& vm_type = vm->types[this->get_index()];

    return (::REType*)vm_type.reflection_type;
#endif
}

void* RETypeDefinition::get_instance() const {
    const auto t = get_type();

    if (t == nullptr) {
        return nullptr;
    }

    return utility::re_type::get_singleton_instance(t);
}

void* RETypeDefinition::create_instance() const {
    const auto t = get_type();

    if (t == nullptr) {
        return nullptr;
    }

    return utility::re_type::create_instance(t);
}
} // namespace sdk