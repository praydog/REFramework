#pragma once

#include <cstdint>
#include <string_view>

#include "RETypeCLR.hpp"
#include "ReClass.hpp"

class REManagedObject;

// Manual definitions of REClassInfo because ReClass doesn't have bitfields like this.
namespace sdk {
struct RETypeDefVersion69;
struct RETypeDefVersion67;
struct RETypeDefVersion66;

struct REField;
struct REMethodDefinition;
struct REProperty;
struct RETypeDefinition;

#ifdef RE8
#define TYPE_INDEX_BITS 18
using RETypeDefinition_ = sdk::RETypeDefVersion69;
#elif defined(RE3) || defined(DMC5)
#define TYPE_INDEX_BITS 17
using RETypeDefinition_ = sdk::RETypeDefVersion67;
#else
#define TYPE_INDEX_BITS 16
using RETypeDefinition_ = sdk::RETypeDefVersion66;
#endif

struct RETypeDefVersion69 {
    uint64_t index : TYPE_INDEX_BITS;
    uint64_t parent_typeid : TYPE_INDEX_BITS;
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t underlying_typeid : 7;
    uint64_t object_type : 3;
    uint64_t array_typeid : TYPE_INDEX_BITS;
    uint64_t element_typeid : TYPE_INDEX_BITS;
    uint64_t impl_index : TYPE_INDEX_BITS;
    uint64_t system_typeid : 10;

    uint32_t type_flags;
    uint32_t size;
    uint32_t fqn_hash;
    uint32_t type_crc;
    uint32_t default_ctor;
    uint32_t vt;
    uint32_t member_method;
    uint32_t member_field;

    // 0x0030
    uint32_t num_member_prop : 12;
    uint32_t member_prop : 19;

    uint32_t member_event;
    int32_t interfaces;
    int32_t generics;
    struct sdk::RETypeCLR* type;
    class ::REObjectInfo* managed_vt;
};

struct RETypeDefVersion67 {
    // 0-8
    uint64_t index : TYPE_INDEX_BITS;
    uint64_t unkbitfieldthing : 13;
    uint64_t parent_typeid : TYPE_INDEX_BITS;
    uint64_t declaring_typeid : TYPE_INDEX_BITS;

    uint32_t fqn_hash; // murmurhash3
    uint32_t type_crc;
    char pad_0010[8];
    uint32_t name_offset;
    uint32_t namespace_offset;
    uint32_t type_flags;
    uint8_t system_type;
    char pad_0025[1];
    uint8_t object_type;
    char pad_0027[1];
    uint32_t default_ctor;
    uint32_t element_size;
    uint32_t size;
    char pad_0034[4];

    // this is fun
#ifdef RE3
    uint32_t member_method;
    uint32_t num_member_method;
    uint32_t member_field;
    uint32_t num_member_field;
#else
    uint32_t num_member_method : 12;
    uint32_t member_method : 19;
    uint32_t num_member_field : 12;
    uint32_t member_field : 19;
#endif

    uint32_t num_member_prop : 12;
    uint32_t member_prop : 19;

    uint32_t events;
    uint32_t interfaces;
    char pad_0054[4];
    uint32_t generics;
    uint32_t vt; // byte pool
    char pad_005C[8];
    void* unk;
    struct sdk::RETypeCLR* type;
    class ::REObjectInfo* managed_vt;
};

struct RETypeDefVersion66 {
    // 0-8
    uint64_t index : TYPE_INDEX_BITS;
    uint64_t unkbitfieldthing : 16;
    uint64_t parent_typeid : TYPE_INDEX_BITS;
    uint64_t declaring_typeid : TYPE_INDEX_BITS;

    uint32_t fqn_hash; // murmurhash3
    uint32_t type_crc;
    char pad_0010[8];
    uint32_t name_offset;
    uint32_t namespace_offset;
    uint32_t type_flags;
    uint8_t system_type;
    char pad_0025[1];
    uint8_t object_type;
    char pad_0027[1];
    uint32_t default_ctor;
    uint32_t element_size;
    uint32_t size;
    char pad_0034[4];

    uint32_t num_member_method : 12;
    uint32_t member_method : 19;
    uint32_t num_member_field : 12;
    uint32_t member_field : 19;
    uint32_t num_member_prop : 12;
    uint32_t member_prop : 19;

    uint32_t events;
    uint32_t interfaces;
    char pad_0054[4];
    uint32_t generics;
    uint32_t vt; // byte pool
    char pad_005C[8];
    void* unk;
    struct sdk::RETypeCLR* type;
    class ::REObjectInfo* managed_vt;
};

#ifndef RE8
#if defined(RE3)
static_assert(sizeof(RETypeDefVersion67) == 0x80);
#else
#if defined(DMC5)
static_assert(sizeof(RETypeDefVersion67) == 0x78);
#else
static_assert(sizeof(RETypeDefVersion66) == 0x78);
#endif
#endif
#endif
} // namespace sdk

// helper class
namespace sdk {
struct RETypeDefinition : public sdk::RETypeDefinition_ {
    class MethodIterator {
    public:
        MethodIterator(const sdk::RETypeDefinition* parent)
            : m_parent{parent} {}

        sdk::REMethodDefinition* begin() const;
        sdk::REMethodDefinition* end() const;
        size_t size() const;

    private:
        const sdk::RETypeDefinition* m_parent;
    };

    class FieldIterator {
    public:
        FieldIterator(const sdk::RETypeDefinition* parent)
            : m_parent{parent} {}

        sdk::REField* begin() const;
        sdk::REField* end() const;
        size_t size() const;

    private:
        const sdk::RETypeDefinition* m_parent;
    };

    class PropertyIterator {
    public:
        PropertyIterator(const sdk::RETypeDefinition* parent)
            : m_parent{parent} {}

        sdk::REProperty* begin() const;
        sdk::REProperty* end() const;

    private:
        const sdk::RETypeDefinition* m_parent;
    };

    MethodIterator get_methods() const { return MethodIterator{this}; }
    FieldIterator get_fields() const { return FieldIterator{this}; }
    PropertyIterator get_properties() const { return PropertyIterator{this}; }

    const char* get_namespace() const;
    const char* get_name() const;

    std::string get_full_name() const;

    sdk::RETypeDefinition* get_declaring_type() const;
    sdk::RETypeDefinition* get_parent_type() const;
    sdk::REField* get_field(std::string_view name) const;
    sdk::REMethodDefinition* get_method(std::string_view name) const;

    int32_t get_fieldptr_offset() const;

    via::clr::VMObjType get_vm_obj_type() const;

    void* get_instance() const;
};
} // namespace sdk