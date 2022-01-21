#ifndef REFRAMEWORK_API_H
#define REFRAMEWORK_API_H

#ifdef REFRAMEWORK_EXPORTS
#define REFRAMEWORK_API __declspec(dllexport)
#else
// don't use this unless you really want to.
#ifdef REFRAMEWORK_IMPORTS
#define REFRAMEWORK_API __declspec(dllimport)
#else
#define REFRAMEWORK_API
#endif
#endif

#define REFRAMEWORK_PLUGIN_VERSION_MAJOR 1
#define REFRAMEWORK_PLUGIN_VERSION_MINOR 0
#define REFRAMEWORK_PLUGIN_VERSION_PATCH 0

#define REFRAMEWORK_RENDERER_D3D11 0
#define REFRAMEWORK_RENDERER_D3D12 1

struct lua_State;

typedef void (*REFInitializedCb)();
typedef void (*REFLuaStateCreatedCb)(lua_State*);
typedef void (*REFLuaStateDestroyedCb)(lua_State*);
typedef void (*REFOnFrameCb)();
typedef void (*REFOnPreApplicationEntryCb)();
typedef void (*REFOnPostApplicationEntryCb)();
typedef void (*REFOnDeviceResetCb)();
typedef bool (*REFOnMessageCb)(void*, unsigned int, unsigned long long, long long);

typedef bool (*REFOnInitializeFn)(REFInitializedCb);
typedef bool (*REFOnLuaStateCreatedFn)(REFLuaStateCreatedCb);
typedef bool (*REFOnLuaStateDestroyedFn)(REFLuaStateDestroyedCb);
typedef bool (*REFOnFrameFn)(REFOnFrameCb);
typedef bool (*REFOnPreApplicationEntryFn)(const char*, REFOnPreApplicationEntryCb);
typedef bool (*REFOnPostApplicationEntryFn)(const char*, REFOnPostApplicationEntryCb);
typedef void (*REFLuaLockUnlockFn)();
typedef bool (*REFOnDeviceResetFn)(REFOnDeviceResetCb);
typedef bool (*REFOnMessageFn)(REFOnMessageCb);

// Optional imports. Passed through REFrameworkPluginFunctions.
REFRAMEWORK_API bool reframework_on_initialized(REFInitializedCb cb);
REFRAMEWORK_API bool reframework_on_lua_state_created(REFLuaStateCreatedCb cb);
REFRAMEWORK_API bool reframework_on_lua_state_destroyed(REFLuaStateDestroyedCb cb);
REFRAMEWORK_API bool reframework_on_frame(REFOnFrameCb cb);
REFRAMEWORK_API bool reframework_on_pre_application_entry(const char* name, REFOnPreApplicationEntryCb cb);
REFRAMEWORK_API bool reframework_on_post_application_entry(const char* name, REFOnPostApplicationEntryCb cb);
REFRAMEWORK_API void reframework_lock_lua();
REFRAMEWORK_API void reframework_unlock_lua();
REFRAMEWORK_API bool reframework_on_device_reset(REFOnDeviceResetCb cb);
REFRAMEWORK_API bool reframework_on_message(REFOnMessageCb cb);

typedef struct {
    int major;
    int minor;
    int patch;
    const char* game_name;
} REFrameworkPluginVersion;

typedef void (*REFPluginRequiredVersionFn)(REFrameworkPluginVersion*);

typedef struct {
    REFOnInitializeFn on_initialized;
    REFOnLuaStateCreatedFn on_lua_state_created;
    REFOnLuaStateDestroyedFn on_lua_state_destroyed;
    REFOnFrameFn on_frame;
    REFOnPreApplicationEntryFn on_pre_application_entry;
    REFOnPostApplicationEntryFn on_post_application_entry;
    REFLuaLockUnlockFn lock_lua;
    REFLuaLockUnlockFn unlock_lua;
    REFOnDeviceResetFn on_device_reset;
    REFOnMessageFn on_message;
} REFrameworkPluginFunctions;

typedef struct {
    int renderer_type;
    void* device;
    void* swapchain;
    void* command_queue;
} REFrameworkRendererData;

// strong typedefs
#define DECLARE_REFRAMEWORK_HANDLE(name) struct name##__ { int unused; }; \
                             typedef struct name##__ *name

DECLARE_REFRAMEWORK_HANDLE(REFrameworkTypeDefinitionHandle);
DECLARE_REFRAMEWORK_HANDLE(REFrameworkMethodHandle);
DECLARE_REFRAMEWORK_HANDLE(REFrameworkFieldHandle);
DECLARE_REFRAMEWORK_HANDLE(REFrameworkPropertyHandle);
DECLARE_REFRAMEWORK_HANDLE(REFrameworkManagedObjectHandle);
DECLARE_REFRAMEWORK_HANDLE(REFrameworkTDBHandle);

#define REFRAMEWORK_CREATE_INSTANCE_FLAGS_NONE 0
#define REFRAMEWORK_CREATE_INSTANCE_FLAGS_SIMPLIFY 1

#define REFRAMEWORK_VM_OBJ_TYPE_NULL 0
#define REFRAMEWORK_VM_OBJ_TYPE_OBJECT 1
#define REFRAMEWORK_VM_OBJ_TYPE_ARRAY 2
#define REFRAMEWORK_VM_OBJ_TYPE_STRING 3
#define REFRAMEWORK_VM_OBJ_TYPE_DELEGATE 4
#define REFRAMEWORK_VM_OBJ_TYPE_VALTYPE 5

typedef struct {
    unsigned int (*get_index)(REFrameworkTypeDefinitionHandle);
    unsigned int (*get_size)(REFrameworkTypeDefinitionHandle);
    unsigned int (*get_valuetype_size)(REFrameworkTypeDefinitionHandle);

    const char* (*get_name)(REFrameworkTypeDefinitionHandle);
    const char* (*get_namespace)(REFrameworkTypeDefinitionHandle);
    bool (*get_full_name)(REFrameworkTypeDefinitionHandle, char* out, unsigned int out_size, unsigned int* out_len);

    bool (*has_fieldptr_offset)(REFrameworkTypeDefinitionHandle);
    int (*get_fieldptr_offset)(REFrameworkTypeDefinitionHandle);

    unsigned int (*get_num_methods)(REFrameworkTypeDefinitionHandle);
    unsigned int (*get_num_fields)(REFrameworkTypeDefinitionHandle);
    unsigned int (*get_num_properties)(REFrameworkTypeDefinitionHandle);

    bool (*is_derived_from)(REFrameworkTypeDefinitionHandle, REFrameworkTypeDefinitionHandle);
    bool (*is_derived_from_by_name)(REFrameworkTypeDefinitionHandle, const char*);
    bool (*is_valuetype)(REFrameworkTypeDefinitionHandle);
    bool (*is_enum)(REFrameworkTypeDefinitionHandle);
    bool (*is_by_ref)(REFrameworkTypeDefinitionHandle);
    bool (*is_pointer)(REFrameworkTypeDefinitionHandle);
    bool (*is_primitive)(REFrameworkTypeDefinitionHandle);

    unsigned int (*get_vm_obj_type)(REFrameworkTypeDefinitionHandle);

    // All lookups are cached on our end
    REFrameworkMethodHandle (*find_method)(REFrameworkTypeDefinitionHandle, const char*);
    REFrameworkFieldHandle (*find_field)(REFrameworkTypeDefinitionHandle, const char*);
    REFrameworkPropertyHandle (*find_property)(REFrameworkTypeDefinitionHandle, const char*); // not implemented yet.

    // out_size is the full size, in bytes of the out buffer
    // out_len is how many elements were written to the out buffer, not the size of the written data
    void (*get_methods)(REFrameworkTypeDefinitionHandle, REFrameworkMethodHandle* out, unsigned int out_size, unsigned int* out_len);
    void (*get_fields)(REFrameworkTypeDefinitionHandle, REFrameworkFieldHandle* out, unsigned int out_size, unsigned int* out_len);

    // get_instance usually only used for native singletons
    void* (*get_instance)(REFrameworkTypeDefinitionHandle);
    void* (*create_instance_deprecated)(REFrameworkTypeDefinitionHandle);
    REFrameworkManagedObjectHandle (*create_instance)(REFrameworkTypeDefinitionHandle, unsigned int flags);

    REFrameworkTypeDefinitionHandle (*get_parent_type)(REFrameworkTypeDefinitionHandle);
    REFrameworkTypeDefinitionHandle (*get_declaring_type)(REFrameworkTypeDefinitionHandle);
    REFrameworkTypeDefinitionHandle (*get_underlying_type)(REFrameworkTypeDefinitionHandle);
} REFrameworkTDBTypeDefinition;

/*
#pragma pack(push, 1)
struct InvokeRet {
    union {
        std::array<uint8_t, 128> bytes{};
        uint8_t byte;
        uint16_t word;
        uint32_t dword;
        float f;
        uint64_t qword;
        double d;
        void* ptr;
    };

    bool exception_thrown{false};
};
#pragma pack(pop)
*/

#define REFRAMEWORK_INVOKE_ERROR_NONE 0
#define REFRAMEWORK_INVOKE_ERROR_EXCEPTION 1
#define REFRAMEWORK_INVOKE_ERROR_OUT_TOO_SMALL 2
#define REFRAMEWORK_INVOKE_ERROR_IN_ARGS_SIZE_MISMATCH 3

typedef void* (*REFGenericFunction)(...);

typedef struct {
    // make sure out size is at least size of InvokeRet
    // each arg is always 8 bytes, even if it's something like a byte
    int (*invoke)(REFrameworkMethodHandle, void* thisptr, void** in_args, unsigned int in_args_size, void* out, unsigned int out_size);
    void* (*get_function)(REFrameworkMethodHandle);
    const char* (*get_name)(REFrameworkMethodHandle);
    REFrameworkTypeDefinitionHandle (*get_return_type)(REFrameworkMethodHandle);
    unsigned int (*get_num_params)(REFrameworkMethodHandle);
    unsigned int (*get_index)(REFrameworkMethodHandle);
    int (*get_virtual_index)(REFrameworkMethodHandle);
    bool (*is_static)(REFrameworkMethodHandle);
    unsigned short (*get_flags)(REFrameworkMethodHandle);
    unsigned short (*get_impl_flags)(REFrameworkMethodHandle);
    unsigned int (*get_invoke_id)(REFrameworkMethodHandle);
} REFrameworkTDBMethod;

typedef struct {
    const char* (*get_name)(REFrameworkFieldHandle);

    REFrameworkTypeDefinitionHandle (*get_declaring_type)(REFrameworkFieldHandle);
    REFrameworkTypeDefinitionHandle (*get_type)(REFrameworkFieldHandle);

    unsigned int (*get_offset_from_base)(REFrameworkFieldHandle);
    unsigned int (*get_offset_from_fieldptr)(REFrameworkFieldHandle);
    unsigned int (*get_flags)(REFrameworkFieldHandle);

    bool (*is_static)(REFrameworkFieldHandle);
    bool (*is_literal)(REFrameworkFieldHandle);

    void* (*get_init_data)(REFrameworkFieldHandle);
    void* (*get_data_raw)(REFrameworkFieldHandle, void* obj, bool is_value_type);
} REFrameworkTDBField;

typedef struct {
    // todo
} REFrameworkTDBProperty;

typedef struct {
    unsigned int (*get_num_types)(REFrameworkTDBHandle);
    unsigned int (*get_num_methods)(REFrameworkTDBHandle);
    unsigned int (*get_num_fields)(REFrameworkTDBHandle);
    unsigned int (*get_num_properties)(REFrameworkTDBHandle);
    unsigned int (*get_strings_size)(REFrameworkTDBHandle);
    unsigned int (*get_raw_data_size)(REFrameworkTDBHandle);
    const char* (*get_string_database)(REFrameworkTDBHandle);
    unsigned char* (*get_raw_database)(REFrameworkTDBHandle);

    // All lookups are cached on our end
    REFrameworkTypeDefinitionHandle (*get_type)(REFrameworkTDBHandle, unsigned int index);
    REFrameworkTypeDefinitionHandle (*find_type)(REFrameworkTDBHandle, const char* name);
    REFrameworkTypeDefinitionHandle (*find_type_by_fqn)(REFrameworkTDBHandle, unsigned int fqn);
    REFrameworkMethodHandle (*get_method)(REFrameworkTDBHandle, unsigned int index);
    REFrameworkMethodHandle (*find_method)(REFrameworkTDBHandle, const char* type_name, const char* name);
    REFrameworkFieldHandle (*get_field)(REFrameworkTDBHandle, unsigned int index);
    REFrameworkFieldHandle (*find_field)(REFrameworkTDBHandle, const char* type_name, const char* name);
    REFrameworkPropertyHandle (*get_property)(REFrameworkTDBHandle, unsigned int index);
} REFrameworkTDB;

typedef struct {
    void (*add_ref)(REFrameworkManagedObjectHandle);
    void (*release)(REFrameworkManagedObjectHandle);
    REFrameworkTypeDefinitionHandle (*get_type_definition)(REFrameworkManagedObjectHandle);
    bool (*is_managed_object)(void*);
    unsigned int (*get_ref_count)(REFrameworkManagedObjectHandle);
} REFrameworkManagedObject;

typedef struct {
    REFrameworkTDBHandle (*get_tdb)();
    void* (*get_vm_context)(); // per-thread context
    REFrameworkManagedObjectHandle (*typeof)(const char*); // System.Type
    REFrameworkManagedObjectHandle (*get_managed_singleton)(const char*);
    void* (*get_native_singleton)(const char*);
} REFrameworkSDKFunctions;

typedef struct {
    const REFrameworkSDKFunctions* functions;
    const REFrameworkTDB* tdb;
    const REFrameworkTDBTypeDefinition* type_definition;
    const REFrameworkTDBMethod* method;
    const REFrameworkTDBField* field;
    const REFrameworkTDBProperty* property;
    const REFrameworkManagedObject* managed_object;
} REFrameworkSDKData;

typedef struct {
    void* reframework_module;
    const REFrameworkPluginVersion* version;
    const REFrameworkPluginFunctions* functions;
    const REFrameworkRendererData* renderer_data;
    const REFrameworkSDKData* sdk;
} REFrameworkPluginInitializeParam;

typedef bool (*REFPluginInitializeFn)(const REFrameworkPluginInitializeParam*);

#endif