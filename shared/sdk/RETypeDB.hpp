#include <string_view>
#include <vector>
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

namespace reframework {
struct InvokeRet;
}

namespace sdk {
struct RETypeDB;
struct RETypeDefinition;
struct RETypeImpl;
struct REField;
struct REFieldImpl;
struct REMethodDefinition;
struct REMethodImpl;
struct REProperty;
struct REPropertyImpl;
struct REParameterDef;

reframework::InvokeRet invoke_object_func(void* obj, sdk::RETypeDefinition* t, std::string_view name, const std::vector<void*>& args);
reframework::InvokeRet invoke_object_func(::REManagedObject* obj, std::string_view name, const std::vector<void*>& args);

sdk::REMethodDefinition* get_object_method(::REManagedObject* obj, std::string_view name);

sdk::RETypeDefinition* find_type_definition(std::string_view type_name);
sdk::RETypeDefinition* find_type_definition_by_fqn(uint32_t fqn);
sdk::REMethodDefinition* find_method_definition(std::string_view type_name, std::string_view method_name);

void* find_native_method(sdk::RETypeDefinition* t, std::string_view method_name);
void* find_native_method(std::string_view type_name, std::string_view method_name);

template <typename T, typename... Args> 
T call_native_func(void* obj, sdk::RETypeDefinition* t, std::string_view name, Args... args);

template <typename T, typename... Args>
T call_native_func_easy(void* obj, sdk::RETypeDefinition* t, std::string_view name, Args... args);

template <typename T, typename... Args> 
T call_object_func(::REManagedObject* obj, std::string_view name, Args... args);

template <typename T, typename... Args>
T call_object_func_easy(::REManagedObject* obj, std::string_view name, Args... args);

template<typename T>
T* get_object_field(void* obj, sdk::RETypeDefinition* t, std::string_view name, bool is_value_type = false);

template<typename T>
T* get_object_field(::REManagedObject* obj, std::string_view name, bool is_value_type = false);

template<typename T>
T* get_static_field(std::string_view type_name, std::string_view name, bool is_value_type = false);

template <typename T = void>
T* get_native_singleton(std::string_view type_name);

template<typename T>
T* get_managed_singleton();

template<typename T, uint32_t Hash>
T* get_managed_singleton();

template<typename T = ::REManagedObject>
T* create_instance(std::string_view type_name, bool simplify = false);
}

// Real meat
#pragma once

#include "utility/String.hpp"

#include "RETypeDefinition.hpp"
#include "REManagedObject.hpp"
#include "REContext.hpp"
#include "TDBVer.hpp"
#include "REGlobals.hpp"

namespace sdk {
namespace tdb71 {
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
    void* unk;
    int32_t (*attributes2)[256];                // 0x00C0 + 8
    char (*stringPool)[1];                      // 0x00C8 + 8
    uint8_t (*bytePool)[256];                   // 0x00D0 + 8
    int32_t (*internStrings)[14154];            // 0x00D8 + 8
};

#pragma pack(push, 4)
struct REParameterDef {
    uint16_t attributes_id;
    uint16_t init_data_index;
    uint32_t name_offset : 30;
    uint32_t modifier : 2;
    uint32_t type_id : TYPE_INDEX_BITS;
    uint32_t flags : (32 - TYPE_INDEX_BITS);
};

struct REMethodDefinition {
    uint32_t declaring_typeid : TYPE_INDEX_BITS;
    uint32_t params_lo : 13;
    uint32_t impl_id : 19;
    uint32_t params_hi : 13;
    int32_t encoded_offset;
};
static_assert(sizeof(REMethodDefinition) == 0xC);

struct REMethodImpl {
    uint16_t attributes_id;
    int16_t vtable_index;
    uint16_t flags;
    uint16_t impl_flags;
    uint32_t name_offset;
};

struct RETypeImpl {
    int32_t name_offset; // 0x0
    int32_t namespace_offset; // 0x4
    int32_t field_size; // 0x8
    int32_t static_field_size; // 0xc
    uint64_t unk_pad : 33; // 0x10
    uint64_t num_member_fields : 24; // 0x10
    uint64_t unk_pad_2 : 7; // 0x10
    uint16_t num_member_methods; // 0x18
    int16_t num_native_vtable; // 0x1a
    int16_t interface_id; // 0x1c
    char pad_1e[0x12];
};
#if TDB_VER >= 71
static_assert(sizeof(RETypeImpl) == 0x30);
static_assert(offsetof(RETypeImpl, num_member_methods) == 0x18);
#endif

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

struct ParamList {
    uint16_t numParams; //0x0000
	uint16_t invokeID; //0x0002
	uint32_t returnType; //0x0004
	uint32_t params[1]; //0x0008
};

struct REField {
    uint64_t declaring_typeid : TYPE_INDEX_BITS;
    uint64_t impl_id : TYPE_INDEX_BITS;
    uint64_t field_typeid : TYPE_INDEX_BITS;
    uint64_t init_data_hi : 6;
    uint64_t rest2 : 1;
};

struct REFieldImpl {
    uint16_t attributes_id;
    uint16_t unk : 1;
    uint16_t flags : 15;
    uint32_t offset : 26;
    uint32_t init_data_lo : 6;
    uint32_t name_offset : 28;
    uint32_t init_data_mid : 4;
};

struct GenericListData {
    uint32_t definition_typeid : TYPE_INDEX_BITS;
    uint32_t num : (32 - TYPE_INDEX_BITS);
    uint32_t types[1];
};
}

namespace tdb70 {
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
    void* unk;
    int32_t (*attributes2)[256];                // 0x00C0 + 8
    char (*stringPool)[1];                      // 0x00C8 + 8
    uint8_t (*bytePool)[256];                   // 0x00D0 + 8
    int32_t (*internStrings)[14154];            // 0x00D8 + 8
};
}

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
    char (*stringPool)[1];                      // 0x00C8
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

struct ParamList {
    uint16_t numParams; //0x0000
	uint16_t invokeID; //0x0002
	uint32_t returnType; //0x0004
	uint32_t params[1]; //0x0008
};

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
    uint32_t unk_idk : 16; // 0x0
    uint32_t invoke_id : 16; // 0x0
    uint16_t declaring_typeid; // 0x4
    uint16_t vtable_index; // 0x6
    uint32_t prototype_name_offset; // 0x8
    char pad_c[0x4];
    uint32_t name_offset; // 0x10
    uint16_t flags; // 0x14
    uint16_t impl_flags; // 0x16
    uint32_t unk2; // 0x18
    uint32_t params; // 0x1c
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
    uint16_t unk_thingy; // 0x10
    char pad_12[0x2];
    uint32_t offset; // 0x14
    uint32_t init_data_offset;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct REMethodParamDef {
    uint16_t num_params;
    uint16_t return_typeid;

    struct Param {
        uint64_t param_typeid : 16;
        uint64_t flags : 16;
        uint64_t name_offset : 31;
    } params[1];
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

#if TDB_VER >= 71
struct RETypeDB_ : public sdk::tdb71::TDB {};

// FIX IT!!!!
struct REMethodDefinition_ : public sdk::tdb71::REMethodDefinition {};
struct REMethodImpl : public sdk::tdb71::REMethodImpl {};
using REField_ = sdk::tdb71::REField;
struct REFieldImpl : public sdk::tdb71::REFieldImpl {};
struct RETypeImpl : public sdk::tdb71::RETypeImpl {};
struct REPropertyImpl : public sdk::tdb71::REPropertyImpl {};
struct REProperty : public sdk::tdb71::REProperty {};
struct REParameterDef : public sdk::tdb71::REParameterDef {};
struct GenericListData : public sdk::tdb71::GenericListData {};
using ParamList = sdk::tdb71::ParamList;
#elif TDB_VER >= 69
#ifdef RE8
struct RETypeDB_ : public sdk::tdb69::TDB {};
#elif defined(MHRISE) || defined(RE2) || defined(RE3) || defined(RE7)
struct RETypeDB_ : public sdk::tdb70::TDB {};
#endif
struct REMethodDefinition_ : public sdk::tdb69::REMethodDefinition {};
struct REMethodImpl : public sdk::tdb69::REMethodImpl {};
using REField_ = sdk::tdb69::REField;
struct REFieldImpl : public sdk::tdb69::REFieldImpl {};
struct RETypeImpl : public sdk::tdb69::RETypeImpl {};
struct REPropertyImpl : public sdk::tdb69::REPropertyImpl {};
struct REProperty : public sdk::tdb69::REProperty {};
struct REParameterDef : public sdk::tdb69::REParameterDef {};
struct GenericListData : public sdk::tdb69::GenericListData {};
using ParamList = sdk::tdb69::ParamList;
#elif TDB_VER == 67
struct RETypeDB_ : public sdk::tdb67::TDB {};
struct REMethodDefinition_ : public sdk::tdb67::REMethodDefinition {};
using REField_ = sdk::tdb67::REField;
struct REProperty : public sdk::tdb67::REProperty {};
struct GenericListData : public sdk::tdb67::GenericListData {};
using REMethodParamDef = sdk::tdb67::REMethodParamDef;
#elif TDB_VER == 66
struct RETypeDB_ : public sdk::tdb66::TDB {};
struct REMethodDefinition_ : public sdk::tdb66::REMethodDefinition {};
using REField_ = sdk::tdb66::REField;
struct REProperty : public sdk::tdb66::REProperty {};
struct GenericListData : public sdk::tdb66::GenericListData {};
using REMethodParamDef = sdk::tdb66::REMethodParamDef;
#elif TDB_VER == 49
struct RETypeDB_ : public sdk::tdb49::TDB {};
struct REMethodDefinition_ : public sdk::tdb49::REMethodDefinition {};
using REField_ = sdk::tdb49::REField;
struct REProperty : public sdk::tdb49::REProperty {};
using REMethodParamDef = sdk::tdb49::REMethodParamDef;

// FIX THIS!!!!
struct GenericListData : public sdk::tdb66::GenericListData {};
#else
static_assert(false, "TDB_VER is not defined");
#endif
} // namespace sdk

namespace sdk {
struct RETypeDB : public sdk::RETypeDB_ {
    static RETypeDB* get();

    sdk::RETypeDefinition* find_type(std::string_view name) const;
    sdk::RETypeDefinition* find_type_by_fqn(uint32_t fqn) const;
    sdk::RETypeDefinition* get_type(uint32_t index) const;
    sdk::REMethodDefinition* get_method(uint32_t index) const;
    sdk::REField* get_field(uint32_t index) const;
    sdk::REProperty* get_property(uint32_t index) const;

    uint32_t get_num_types() const {
        return numTypes;
    }

    uint32_t get_num_methods() const {
        return numMethods;
    }

    uint32_t get_num_fields() const {
        return numFields;
    }

#if TDB_VER >= 69
    uint32_t get_num_params() const {
        return numParams;
    }
#endif

    uint32_t get_num_properties() const {
        return numProperties;
    }

    uint32_t get_string_pool_size() const {
        return numStringPool;
    }

    uint32_t get_byte_pool_size() const {
        return numBytePool;
    }

    const char* get_string(uint32_t offset) const;
    uint8_t* get_bytes(uint32_t offset) const;

    template <typename T> T* get_data(uint32_t offset) const { return (T*)get_bytes(offset); }

    uint32_t get_string_pool_bitmask() const {
        static auto result = [this]() -> uint32_t {
            uint32_t out{1};
            while (out < get_string_pool_size()) {
                out <<= 1;
            }

            return out - 1;
        }();

        return result;
    }

    uint32_t get_byte_pool_bitmask() const {
        static auto result = [this]() -> uint32_t {
            uint32_t out{1};
            while (out < get_byte_pool_size()) {
                out <<= 1;
            }

            return out - 1;
        }();

        return result;
    }

    uint32_t get_type_bitmask() const {
        static auto result = [this]() -> uint32_t {
            uint32_t out{1};
            while (out < get_num_types()) {
                out <<= 1;
            }

            return out - 1;
        }();

        return result;
    }

#if TDB_VER >= 69
    uint32_t get_param_bitmask() const {
        static auto result = [this]() -> uint32_t {
            uint32_t out{1};
            while (out < get_num_params()) {
                out <<= 1;
            }

            return out - 1;
        }();

        return result;
    }
#endif
};
} // namespace sdk

namespace sdk {
struct REField : public sdk::REField_ {
    sdk::RETypeDefinition* get_declaring_type() const;
    sdk::RETypeDefinition* get_type() const;
    const char* get_name() const;
    uint32_t get_flags() const;
    uint32_t get_init_data_index() const;
    void* get_init_data() const;
    uint32_t get_offset_from_fieldptr() const;
    uint32_t get_offset_from_base() const;
    bool is_static() const;
    bool is_literal() const;

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
    bool is_static() const;
    
    template<typename T>
    T get_function_t() const {
        return (T)get_function();
    }

    template<typename T = void*, typename ...Args>
    T call(Args... args) const {
        return get_function_t<T (*)(Args...)>()(args...);
    }

    // Does what invoke does without all the stupid setup beforehand
    template<typename T = void*, typename ...Args>
    T call_safe(Args... args) const {
        if constexpr (std::is_same_v<T, void>) {
            sdk::VMContext::safe_wrap(get_name(), [&]() {
                get_function_t<void (*)(Args...)>()(args...);
            });
            return;
        }
        
        if constexpr (!std::is_same_v<T, void>) {
            T result{};
            sdk::VMContext::safe_wrap(get_name(), [&]() {
                result = get_function_t<T (*)(Args...)>()(args...);
            });

            return result;
        }
    }

    template <typename Ret = void*, typename ...Types>
    struct CallHelper {
        template<size_t... I>
        struct Dispatcher {
            Dispatcher(std::tuple<Types...>&& args) : args(args) {}

            template<typename ...PreambleTypes>
            auto operator()(const REMethodDefinition* target, PreambleTypes... preamble) {
                return target->call<Ret>(preamble..., std::get<I>(args)...);
            }

            std::tuple<Types...> args;
        };

        template <size_t N, typename S = std::make_index_sequence<N>>
        struct Packer {
            template<size_t... I>
            static auto pack(void** voids, std::index_sequence<I...>) {
                return Dispatcher<I...>{std::make_tuple(*(Types*)&voids[I]...)};
            }
        
            static auto pack(void** voids) {
                return pack(voids, S{});
            }
        };

        static auto create(void** voids) {
            if constexpr (sizeof...(Types) > 0) {
                return Packer<sizeof...(Types)>::pack(voids);
            } else {
                return std::make_tuple<Types...>();
            }
        };
    };

    template <typename ...Types>
    struct CallHelperRet {
        template<size_t... I>
        struct Dispatcher {
            Dispatcher(const REMethodDefinition* t, auto&& args) 
            : target{t}, 
            args(args) 
            {

            }

            template<typename T>
            operator T() {
                if constexpr (sizeof(T) > sizeof(void*)) {
                    T out{};
                    target->call<T>(&out, std::get<I>(args)...);
                    return out;
                }

                return target->call<T>(std::get<I>(args)...);
            }

            std::tuple<Types...> args;
            const REMethodDefinition* target;
        };

        template <size_t N, typename S = std::make_index_sequence<N>>
        struct Packer {
            template<size_t... I>
            static auto pack(const sdk::REMethodDefinition* target, auto&& args, std::index_sequence<I...>) {
                return Dispatcher<I...>{target, args};
            }

            static auto pack(const sdk::REMethodDefinition* target, auto&& args) {
                return pack(target, args, S{});
            }
        };

        static auto create(const sdk::REMethodDefinition* target, Types... args) {
            return Packer<sizeof...(Types)>::pack(target, std::make_tuple(args...));
        }
    };

    template <typename... Args> 
    auto operator()(Args... args) const {
        return CallHelperRet<Args...>::create(this, args...);
    }

    // calling and invoking are two different things
    // calling is the actual call to the function
    // invoking is calling a wrapper function that calls the function
    // using an array of arguments
    ::reframework::InvokeRet invoke(void* object, const std::vector<void*>& args) const;

    uint32_t get_invoke_id() const;
    uint32_t get_num_params() const;
    uint32_t get_param_index() const {
#if TDB_VER >= 71
        const auto params_index = (this->params_hi << 13) | this->params_lo;
#else
        const auto params_index = this->params;
#endif

        return params_index;
    }

    std::vector<uint32_t> get_param_typeids() const;
    std::vector<sdk::RETypeDefinition*> get_param_types() const;
    std::vector<const char*> get_param_names() const;
};

template <typename T, typename... Args> 
T call_native_func(void* obj, sdk::RETypeDefinition* t, std::string_view name, Args... args) {
    const auto method = t->get_method(name);

    if (method == nullptr) {
        // spdlog::error("Cannot find {:s}", name.data());
        return T{};
    }

    return method->call<T>(args...);
}

template <typename T, typename... Args>
T call_native_func_easy(void* obj, sdk::RETypeDefinition* t, std::string_view name, Args... args) {
    if constexpr (sizeof(T) > sizeof(void*)) {
        T out{};
        call_native_func<T*>((void*)obj, t, name, &out, sdk::get_thread_context(), obj, args...);

        return out;
    }

    // todo, fix statics?
    return call_native_func<T>((void*)obj, t, name, sdk::get_thread_context(), obj, args...);
}

template <typename T, typename... Args> 
T call_object_func(::REManagedObject* obj, std::string_view name, Args... args) {
    auto def = utility::re_managed_object::get_type_definition(obj);

    return call_native_func<T>((void*)obj, def, name, args...);
}

template <typename T, typename... Args> 
T call_object_func_easy(::REManagedObject* obj, std::string_view name, Args... args) {
    if constexpr (sizeof(T) > sizeof(void*)) {
        auto def = utility::re_managed_object::get_type_definition(obj);

        T out{};
        call_native_func<T*>((void*)obj, def, name, &out, sdk::get_thread_context(), obj, args...);

        return out;
    }

    auto def = utility::re_managed_object::get_type_definition(obj);
    return call_native_func<T>((void*)obj, def, name, sdk::get_thread_context(), obj, args...);
}

template<typename T>
T* get_native_field(void* obj, sdk::RETypeDefinition* t, std::string_view name, bool is_value_type) {
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

    return get_native_field<T>((void*)obj, def, name, is_value_type);
}

template<typename T>
T* get_static_field(std::string_view type_name, std::string_view name, bool is_value_type) {
    const auto t = sdk::find_type_definition(type_name);

    if (t == nullptr) {
        // spdlog::error("Cannot find type {:s}", type_name.data());
        return nullptr;
    }

    return get_native_field<T>((void*)nullptr, t, name, is_value_type);
}

template <typename T>
T* get_native_singleton(std::string_view type_name)  {
    const auto t = sdk::find_type_definition(type_name);

    if (t != nullptr) {
        const auto result = t->get_instance();

        if (result != nullptr) {
            return (T*)result;
        }
    }

    const auto retype = reframework::get_globals()->get_native(type_name);
    if (retype == nullptr)  {
        return nullptr;
    }

    const auto instance = utility::re_type::get_singleton_instance(retype);
    if (instance == nullptr)  {
        return nullptr;
    }

    return (T*)instance;
}

template <typename T>
T* get_managed_singleton(std::string_view type_name) {
    auto t = sdk::find_type_definition(type_name);

    if (t == nullptr) {
        //spdlog::error("Cannot find type {:s}", type_name.data());
        return nullptr;
    }

    auto get_instance_method = t->get_method("get_Instance");

    if (get_instance_method == nullptr) {
        //spdlog::error("Cannot find get_Instance method");
        return nullptr;
    }

    return (T*)get_instance_method->call<T*>(sdk::get_thread_context());
}

// FNV-1A
template<typename T, uint32_t Hash>
T* get_managed_singleton() {
    static auto t = []() -> sdk::RETypeDefinition* {
        const auto tdb = sdk::RETypeDB::get();

        for (auto i = 0; i < tdb->numTypes; i++) {
            auto t = tdb->get_type(i);

            if (t == nullptr) {
                continue;
            }

            if (utility::hash(t->get_full_name()) == Hash) {
                return t;
            }
        }

        return nullptr;
    }();

    if (t == nullptr) {
        //spdlog::error("Cannot find type {:s}", type_name.data());
        return nullptr;
    }

    static auto get_instance_method = t->get_method("get_Instance");

    if (get_instance_method == nullptr) {
        //spdlog::error("Cannot find get_Instance method");
        return nullptr;
    }

    return get_instance_method->call<T*>(sdk::get_thread_context());
}

template<typename T>
T* create_instance(std::string_view type_name, bool simplify) {
    auto t = sdk::find_type_definition(type_name);

    if (t == nullptr) {
        //spdlog::error("Cannot find type {:s}", type_name.data());
        return nullptr;
    }

    return (T*)t->create_instance_full(simplify);
}
} // namespace sdk