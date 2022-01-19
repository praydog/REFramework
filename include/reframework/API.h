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

struct lua_State;

typedef void (*REFInitializedCb)();
typedef void (*REFLuaStateCreatedCb)(lua_State*);
typedef void (*REFLuaStateDestroyedCb)(lua_State*);
typedef void (*REFOnFrameCb)();
typedef void (*REFOnPreApplicationEntryCb)();
typedef void (*REFOnPostApplicationEntryCb)();
typedef void (*REFOnDeviceResetCb)();

typedef bool (*REFOnInitializeFn)(REFInitializedCb);
typedef bool (*REFOnLuaStateCreatedFn)(REFLuaStateCreatedCb);
typedef bool (*REFOnLuaStateDestroyedFn)(REFLuaStateDestroyedCb);
typedef bool (*REFOnFrameFn)(REFOnFrameCb);
typedef bool (*REFOnPreApplicationEntryFn)(const char*, REFOnPreApplicationEntryCb);
typedef bool (*REFOnPostApplicationEntryFn)(const char*, REFOnPostApplicationEntryCb);
typedef void (*REFLuaLockUnlockFn)();
typedef bool (*REFOnDeviceResetFn)(REFOnDeviceResetCb);

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

typedef struct {
    int major;
    int minor;
    int patch;
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
} REFrameworkPluginFunctions;

#define REFRAMEWORK_RENDERER_D3D11 0
#define REFRAMEWORK_RENDERER_D3D12 1

typedef struct {
    int renderer_type;
    void* device;
    void* swapchain;
    void* command_queue;
} REFrameworkRendererData;

typedef struct {
    void* reframework_module;
    REFrameworkPluginVersion* version;
    REFrameworkPluginFunctions* functions;
    REFrameworkRendererData* renderer_data;
} REFrameworkPluginInitializeParam;

typedef bool (*REFPluginInitializeFn)(const REFrameworkPluginInitializeParam*);

#endif