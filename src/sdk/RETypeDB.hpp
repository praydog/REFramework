#include <string_view>
#include <cstdint>

// Forward decls
class RETypeDB;
class REClassInfo;
class RETypeImpl;
class REMethodImpl;
class REPropertyImpl;
class REParameterDef;
class REAttributeDef;
class REManagedObject;

namespace sdk {
struct RETypeDefinition;
struct RETypeImpl;
struct REField;
struct REFieldImpl;
struct REMethodDefinition;
struct REMethodImpl;
struct REProperty;
struct REPropertyImpl;
struct REParameterDef;

template <typename T, typename... Args> 
T call_object_func(void* obj, sdk::RETypeDefinition* t, std::string_view name, Args... args);

template <typename T, typename... Args> 
T call_object_func(::REManagedObject* obj, std::string_view name, Args... args);

template<typename T>
T* get_object_field(void* obj, sdk::RETypeDefinition* t, std::string_view name, bool is_value_type = false);

template<typename T>
T* get_object_field(::REManagedObject* obj, std::string_view name, bool is_value_type = false);

static void* find_native_method(sdk::RETypeDefinition* t, std::string_view method_name);
static void* find_native_method(std::string_view type_name, std::string_view method_name);

template <typename T = void>
T* get_native_singleton(std::string_view type_name);
}

// Real meat
#pragma once

#include "RETypeDefinition.hpp"
#include "REManagedObject.hpp"

namespace sdk {
namespace tdb69 {
// todo bring these in from reclass
struct REMethodDefinition;
struct REMethodImpl;
struct REField;
struct REFieldImpl;
struct REProperty;
struct RETypeImpl;
struct REPropertyImpl;
struct REParameterDef;

struct TDB {
    uint32_t magic;                             // 0x0000
    uint32_t version;                           // 0x0004
    uint32_t initialized;                       // 0x0008
    uint32_t numTypes;                          // 0x000C
    uint32_t numMethods;                        // 0x0010
    uint32_t numFields;                         // 0x0014
    uint32_t numTypeImpl;                       // 0x0018
    uint32_t numFieldImpl;                      // 0x001C
    uint32_t numMethodImpl;                     // 0x0020
    uint32_t numPropertyImpl;                   // 0x0024
    uint32_t numProperties;                     // 0x0028
    uint32_t numEvents;                         // 0x002C
    uint32_t numParams;                         // 0x0030
    uint32_t numAttributes;                     // 0x0034
    int32_t numInitData;                        // 0x0038
    uint32_t numAttributes2;                    // 0x003C
    uint32_t numInternStrings;                  // 0x0040
    uint32_t numModules;                        // 0x0044
    int32_t devEntry;                           // 0x0048
    int32_t appEntry;                           // 0x004C
    uint32_t numStringPool;                     // 0x0050
    uint32_t numBytePool;                       // 0x0054
    void* modules;                              // 0x0058
    sdk::RETypeDefinition (*types)[93788];      // 0x0060
    sdk::RETypeImpl (*typesImpl)[256];          // 0x0068
    sdk::REMethodDefinition (*methods)[703558]; // 0x0070
    sdk::REMethodImpl (*methodsImpl)[56756];    // 0x0078
    sdk::REField (*fields)[1];                  // 0x0080
    sdk::REFieldImpl (*fieldsImpl)[1];          // 0x0088
    sdk::REProperty (*properties)[256];         // 0x0090
    sdk::REPropertyImpl (*propertiesImpl)[1];   // 0x0098
    void* events;                               // 0x00A0
    sdk::REParameterDef (*params)[10000];       // 0x00A8
    class ::REAttributeDef (*attributes)[2000]; // 0x00B0
    int32_t (*initData)[19890];                 // 0x00B8
    int32_t (*attributes2)[256];                // 0x00C0
    char (*stringPool)[0];                      // 0x00C8
    uint8_t (*bytePool)[256];                   // 0x00D0
    int32_t (*internStrings)[14154];            // 0x00D8
};

#pragma pack(push, 4)
struct REParameterDef {
    uint16_t attributes_id;
    uint16_t init_data_index;
    uint32_t name_offset : 30;
    uint32_t modifier : 2;
    uint32_t type_id : 18;
    uint32_t flags : 14;
};

struct REMethodDefinition {
    uint64_t declaring_typeid : 18;
    uint64_t impl_id : 20;
    uint64_t params : 26;
    void* function;
};

struct REMethodImpl {
    uint16_t attributes_id;
    int16_t vtable_index;
    uint16_t flags;
    uint16_t impl_flags;
    uint32_t name_offset;
};

struct RETypeImpl {
    int32_t name_offset;
    int32_t namespace_offset;
    int32_t field_size;
    int32_t static_field_size;
    uint8_t module_id;
    uint8_t array_rank;
    uint16_t num_member_methods;
    int32_t num_member_fields;
    int16_t interface_id;
    uint16_t num_native_vtable;
    uint16_t attributes_id;
    uint16_t num_vtable;
    uint64_t mark;
    uint64_t cycle;
};

struct REProperty {
    uint64_t impl_id : 20;
    uint64_t getter : 22;
    uint64_t setter : 22;
};

struct REPropertyImpl {
    uint16_t flags;
    uint16_t attributes_id;
    int32_t name_offset;
};
#pragma pack(pop)

struct REField {
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t impl_id : 20;
    uint64_t offset : 26;
};

struct REFieldImpl {
    uint16_t attributes_id;
    uint16_t flags;
    uint32_t field_typeid : 18;
    uint32_t init_data_lo : 14;
    uint32_t name_offset : 30;
    uint32_t init_data_hi : 2;
};

struct GenericListData {
    uint32_t definition_typeid : TYPE_INDEX_BITS;
    uint32_t num : 14;
    uint32_t types[1];
};
} // namespace tdb69

namespace tdb67 {
struct REMethodDefinition;
struct REField;
struct REProperty;

struct TDB {
    uint32_t magic;                             // 0x0000
    uint32_t version;                           // 0x0004
    uint32_t initialized;                       // 0x0008
    uint32_t numTypes;                          // 0x000C
    uint32_t numMethods;                        // 0x0010
    uint32_t numFields;                         // 0x0014
    uint32_t numProperties;                     // 0x0018
    uint32_t numEvents;                         // 0x001C
    uint32_t numUnk;                            // 0x0020
    uint32_t maybeNumParams;                    // 0x0024
    uint32_t maybeNumAttributes;                // 0x0028
    uint32_t numInitData;                       // 0x002C
    uint32_t numInternStrings;                  // 0x0030
    uint32_t numModules;                        // 0x0034
    uint32_t devEntry;                          // 0x0038
    uint32_t appEntry;                          // 0x003C
    uint32_t numStringPool;                     // 0x0040
    uint32_t numBytePool;                       // 0x0044
    class N00002524 (*modules)[256];            // 0x0048
    sdk::RETypeDefinition (*types)[81728];      // 0x0050
    sdk::REMethodDefinition (*methods)[556344]; // 0x0058
    sdk::REField (*fields)[122496];             // 0x0060
    sdk::REProperty (*properties)[119791];      // 0x0068
    void* events;                               // 0x0070
    char pad_0078[8];                           // 0x0078
    void* N0000243D;                            // 0x0080
    int32_t (*initData)[1];                     // 0x0088
    void* N0000243F;                            // 0x0090
    char (*stringPool)[0];                      // 0x0098
    uint8_t (*bytePool)[1];                     // 0x00A0
    uint32_t (*internStrings)[17014];           // 0x00A8
};

struct REMethodDefinition {
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t invoke_id : 16;
    uint64_t num_params : 6;
    uint64_t unk : 8; // NOT REALLY SURE WHAT THIS IS? IT HAS SOMETHING TO DO WITH RETURN TYPE
    uint64_t return_typeid : TYPE_INDEX_BITS;
    char pad_0008[2];
    int16_t vtable_index;
    uint32_t name_offset;
    uint16_t flags;
    uint16_t impl_flags;
    uint32_t params; // bytepool
    void* function;
};

struct REMethodParamDef {
    uint64_t param_typeid : TYPE_INDEX_BITS;
    uint64_t flags : 16;
    uint64_t name_offset : 31;
};

struct REField {
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t field_typeid : TYPE_INDEX_BITS;
    // TODO: fill in rest of bitfield

    uint32_t name_offset;
    uint16_t flags;
    uint16_t init_data_index;
    uint32_t offset;
    uint32_t unk2;
};

struct REProperty {
    char pad_0000[4];     // 0x0000
    uint32_t name_offset; // 0x0004
    uint32_t getter;      // 0x0008
    uint32_t setter;      // 0x000C
};

struct GenericListData {
    uint32_t definition_typeid : TYPE_INDEX_BITS;
    uint32_t num : 14;
    uint32_t types[1];
};
} // namespace tdb67

namespace tdb66 {
struct REMethodDefinition;
struct REField;
struct REProperty;

struct TDB {
    uint32_t magic;                             // 0x0000
    uint32_t version;                           // 0x0004
    uint32_t initialized;                       // 0x0008
    uint32_t numTypes;                          // 0x000C
    uint32_t numMethods;                        // 0x0010
    uint32_t numFields;                         // 0x0014
    uint32_t numProperties;                     // 0x0018
    uint32_t numEvents;                         // 0x001C
    uint32_t numUnk;                            // 0x0020
    uint32_t maybeNumParams;                    // 0x0024
    uint32_t maybeNumAttributes;                // 0x0028
    uint32_t numInitData;                       // 0x002C
    uint32_t numInternStrings;                  // 0x0030
    uint32_t numModules;                        // 0x0034
    uint32_t devEntry;                          // 0x0038
    uint32_t appEntry;                          // 0x003C
    uint32_t numStringPool;                     // 0x0040
    uint32_t numBytePool;                       // 0x0044
    class N00002524 (*modules)[256];            // 0x0048
    sdk::RETypeDefinition (*types)[81728];      // 0x0050
    sdk::REMethodDefinition (*methods)[556344]; // 0x0058
    sdk::REField (*fields)[122496];             // 0x0060
    sdk::REProperty (*properties)[119791];      // 0x0068
    void* events;                               // 0x0070
    char pad_0078[8];                           // 0x0078
    void* N0000243D;                            // 0x0080
    int32_t (*initData)[1];                     // 0x0088
    void* N0000243F;                            // 0x0090
    char (*stringPool)[0];                      // 0x0098
    uint8_t (*bytePool)[1];                     // 0x00A0
    uint32_t (*internStrings)[17014];           // 0x00A8
};

struct REMethodDefinition {
    uint64_t declaring_typeid : TYPE_INDEX_BITS; // 0 - 2
    int64_t vtable_index : 16;                   // 2 - 4
    uint64_t num_params : 8;                     // 4 - 5
    uint64_t unk : 8;                            // NOT REALLY SURE WHAT THIS IS? IT HAS SOMETHING TO DO WITH RETURN TYPE // 5 - 6
    uint64_t return_typeid : TYPE_INDEX_BITS;
    char pad_0008[2];
    int16_t invoke_id;
    uint32_t name_offset;
    uint16_t flags;
    uint16_t impl_flags;
    uint32_t params; // bytepool
    void* function;
};

struct REMethodParamDef {
    uint64_t param_typeid : TYPE_INDEX_BITS;
    uint64_t flags : 16;
    uint64_t name_offset : 31;
};

#pragma pack(push, 4)
struct REField {
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t field_typeid : TYPE_INDEX_BITS;
    // TODO: fill in rest of bitfield

    uint32_t name_offset;
    uint16_t flags;
    uint16_t init_data_index;
    uint32_t offset;
};
#pragma pack(pop)

static_assert(sizeof(REField) == 0x14);
static_assert(offsetof(REField, name_offset) == 0x8);

struct REProperty {
    char pad_0000[4];     // 0x0000
    uint32_t name_offset; // 0x0004
    uint32_t getter;      // 0x0008
    uint32_t setter;      // 0x000C
};

struct GenericListData {
    uint32_t definition_typeid : TYPE_INDEX_BITS;
    uint32_t num : 16;
    uint16_t types[1];
};
} // namespace tdb66

namespace tdb49 {
struct REMethodDefinition;
struct REField;
struct REProperty;

#pragma pack(push, 1)
struct REProperty {
    uint16_t declaring_typeid; // 0x0
    char pad_2[0x6];
    uint32_t name_offset; // 0x8
    uint32_t getter; // 0xc
    uint32_t setter; // 0x10
};
#pragma pack(pop)

#pragma pack(push, 1)
struct REMethodDefinition {
    struct AdditionalData {
        int16_t unk; // 0x0
        uint16_t return_typeid; // 0x2
    }; // Size: 0x4

    uint64_t declaring_typeid : 16; // 0x0
    uint64_t invoke_id : 16; // 0x0
    uint64_t num_params : 6; // 0x0
    uint64_t unk : 10; // 0x0
    uint64_t vtable_index : 16; // 0x0
    uint32_t prototype_name_offset; // 0x8
    char pad_c[0x4];
    uint32_t name_offset; // 0x10
    uint16_t flags; // 0x14
    uint16_t impl_flags; // 0x16
    uint32_t unk2; // 0x18
    uint32_t method_data_offset; // 0x1c
};
#pragma pack(pop)
static_assert(sizeof(tdb49::REMethodDefinition) == 0x20);
static_assert(offsetof(tdb49::REMethodDefinition, name_offset) == 0x10);

#pragma pack(push, 1)
struct REField {
    uint64_t declaring_typeid : 16; // 0x0
    uint64_t field_typeid : 16; // 0x0
    uint32_t name_offset; // 0x8
    uint16_t flags; // 0xc
    char pad_e[0x2];
    uint16_t init_data_index; // 0x10
    char pad_12[0x2];
    uint32_t offset; // 0x14
    char pad_18[0x4];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TDB {
    uint32_t magic; // 0x0
    uint32_t version; // 0x4
    uint32_t initialized; // 0x8
    uint32_t numTypes; // 0xc
    uint32_t numMethods; // 0x10
    uint32_t numFields; // 0x14
    uint32_t numProperties; // 0x18
    uint32_t numEvents; // 0x1c
    uint32_t numInitData; // 0x20
    uint32_t numModules; // 0x24
    uint32_t devEntry; // 0x28
    uint32_t appEntry; // 0x2c
    uint32_t numStringPool; // 0x30
    uint32_t numBytePool; // 0x34
    void* modules; // 0x38
    sdk::RETypeDefinition (*types)[1]; // 0x40
    sdk::REMethodDefinition (*methods)[1]; // 0x48
    sdk::REField (*fields)[1]; // 0x50
    sdk::REProperty (*properties)[1]; // 0x58
    void* (*events)[0];
    char (*stringPool)[0];
    uint8_t (*bytePool)[1];
    char pad_78[0x88];
};
#pragma pack(pop)
}

#ifdef RE8
struct RETypeDB_ : public sdk::tdb69::TDB {};
struct REMethodDefinition_ : public sdk::tdb69::REMethodDefinition {};
struct REMethodImpl : public sdk::tdb69::REMethodImpl {};
using REField_ = sdk::tdb69::REField;
struct REFieldImpl : public sdk::tdb69::REFieldImpl {};
struct RETypeImpl : public sdk::tdb69::RETypeImpl {};
struct REPropertyImpl : public sdk::tdb69::REPropertyImpl {};
struct REProperty : public sdk::tdb69::REProperty {};
struct REParameterDef : public sdk::tdb69::REParameterDef {};
using GenericListData = sdk::tdb69::GenericListData;
#elif defined(RE3) || defined(DMC5)
struct RETypeDB_ : public sdk::tdb67::TDB {};
struct REMethodDefinition_ : public sdk::tdb67::REMethodDefinition {};
using REField_ = sdk::tdb67::REField;
struct REProperty : public sdk::tdb67::REProperty {};
using GenericListData = sdk::tdb67::GenericListData;
using REMethodParamDef = sdk::tdb67::REMethodParamDef;
#elif RE2
struct RETypeDB_ : public sdk::tdb66::TDB {};
struct REMethodDefinition_ : public sdk::tdb66::REMethodDefinition {};
using REField_ = sdk::tdb66::REField;
struct REProperty : public sdk::tdb66::REProperty {};
using GenericListData = sdk::tdb66::GenericListData;
using REMethodParamDef = sdk::tdb66::REMethodParamDef;
#elif RE7
struct RETypeDB_ : public sdk::tdb49::TDB {};
struct REMethodDefinition_ : public sdk::tdb49::REMethodDefinition {};
using REField_ = sdk::tdb49::REField;
struct REProperty : public sdk::tdb49::REProperty {};

// FIX THIS!!!!
using GenericListData = sdk::tdb66::GenericListData;
using REMethodParamDef = sdk::tdb66::REMethodParamDef;
#endif
} // namespace sdk

namespace sdk {
struct RETypeDB : public sdk::RETypeDB_ {
    static RETypeDB* get();

    sdk::RETypeDefinition* find_type(std::string_view name) const;
    sdk::RETypeDefinition* get_type(uint32_t index) const;
    sdk::REMethodDefinition* get_method(uint32_t index) const;
    sdk::REField* get_field(uint32_t index) const;
    sdk::REProperty* get_property(uint32_t index) const;

    const char* get_string(uint32_t offset) const;
    uint8_t* get_bytes(uint32_t offset) const;

    template <typename T> T* get_data(uint32_t offset) const { return (T*)get_bytes(offset); }
};
} // namespace sdk

namespace sdk {
struct REField : public sdk::REField_ {
    sdk::RETypeDefinition* get_declaring_type() const;
    sdk::RETypeDefinition* get_type() const;
    const char* get_name() const;
    uint32_t get_flags() const;
    void* get_init_data() const;
    uint32_t get_offset_from_fieldptr() const;
    uint32_t get_offset_from_base() const;

    void* get_data_raw(void* object = nullptr, bool is_value_type = false) const;

    template <typename T> T& get_data(void* object = nullptr, bool is_value_type = false) const { return *(T*)get_data_raw(object); }
};

struct REMethodDefinition : public sdk::REMethodDefinition_ {
    sdk::RETypeDefinition* get_declaring_type() const;
    sdk::RETypeDefinition* get_return_type() const;

    const char* get_name() const;
    void* get_function() const;

    uint32_t get_index() const;
    int32_t get_virtual_index() const;
    uint16_t get_flags() const;
    uint16_t get_impl_flags() const;
    
    template<typename T>
    T get_function_t() const {
        return (T)get_function();
    }

    template<typename T = void*, typename ...Args>
    T call(Args... args) const {
        return get_function_t<T (*)(Args...)>()(args...);
    }

    template <typename T = void, typename... Args> 
    T operator()(Args... args) const { 
        return get_function_t<T (*)(Args...)>()(args...); 
    }

    std::vector<uint32_t> get_param_typeids() const;
    std::vector<sdk::RETypeDefinition*> get_param_types() const;
    std::vector<const char*> get_param_names() const;
};

template <typename T, typename... Args> 
T call_object_func(void* obj, sdk::RETypeDefinition* t, std::string_view name, Args... args) {
    const auto method = t->get_method(name);

    if (method == nullptr) {
        // spdlog::error("Cannot find {:s}", name.data());
        return T{};
    }

    return method->call<T>(args...);
}

template <typename T, typename... Args> 
T call_object_func(::REManagedObject* obj, std::string_view name, Args... args) {
    auto def = utility::re_managed_object::get_type_definition(obj);

    return call_object_func<T>((void*)obj, def, name, args...);
}

template<typename T>
T* get_object_field(void* obj, sdk::RETypeDefinition* t, std::string_view name, bool is_value_type) {
    const auto field = t->get_field(name);

    if (field == nullptr) {
        // spdlog::error("Cannot find {:s}", name.data());
        return nullptr;
    }

    return (T*)field->get_data_raw(obj, is_value_type);
}

template<typename T>
T* get_object_field(::REManagedObject* obj, std::string_view name, bool is_value_type) {
    auto def = utility::re_managed_object::get_type_definition(obj);

    return get_object_field<T>((void*)obj, def, name, is_value_type);
}

static void* find_native_method(sdk::RETypeDefinition* t, std::string_view method_name) {
    const auto method = t->get_method(method_name);

    if (method == nullptr) {
        // spdlog::error("Cannot find {:s}", method_name.data());
        return nullptr;
    }

    return method->get_function();
}

static void* find_native_method(std::string_view type_name, std::string_view method_name) {
    auto t = sdk::RETypeDB::get()->find_type(type_name);

    if (t == nullptr) {
        //spdlog::error("Cannot find type {:s}", type_name.data());
        return nullptr;
    }

    return find_native_method(t, method_name);
}

template <typename T>
T* get_native_singleton(std::string_view type_name) {
    auto t = sdk::RETypeDB::get()->find_type(type_name);

    if (t == nullptr) {
        //spdlog::error("Cannot find type {:s}", type_name.data());
        return nullptr;
    }

    return (T*)t->get_instance();
}
} // namespace sdk