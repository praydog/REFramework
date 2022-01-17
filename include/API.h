#ifndef REFRAMEWORK_API_H
#define REFRAMEWORK_API_H

#ifdef REFRAMEWORK_EXPORTS
#define REFRAMEWORK_API __declspec(dllexport)
#else
#define REFRAMEWORK_API __declspec(dllimport)
#endif

struct lua_State;

typedef void (*REFInitializedCb)();
typedef void (*REFLuaStateCreatedCb)(lua_State*);
typedef void (*REFLuaStateDestroyedCb)(lua_State*);
typedef void (*REFOnFrameCb)();
typedef void (*REFOnPreApplicationEntryCb)();
typedef void (*REFOnPostApplicationEntryCb)();

typedef struct {
    int major;
    int minor;
    int patch;
} REFrameworkPluginVersion;

typedef void (*REFPluginRequiredVersionFn)(REFrameworkPluginVersion*);

typedef struct {
    void* reframework_module;
    REFrameworkPluginVersion plugin_version;
} REFrameworkPluginInitializeParam;

typedef bool (*REFPluginInitializeFn)(const REFrameworkPluginInitializeParam*);

REFRAMEWORK_API bool reframework_on_initialized(REFInitializedCb cb);
REFRAMEWORK_API bool reframework_on_lua_state_created(REFLuaStateCreatedCb cb);
REFRAMEWORK_API bool reframework_on_lua_state_destroyed(REFLuaStateDestroyedCb cb);
REFRAMEWORK_API bool reframework_on_frame(REFOnFrameCb cb);
REFRAMEWORK_API bool reframework_on_pre_application_entry(const char* name, REFOnPreApplicationEntryCb cb);
REFRAMEWORK_API bool reframework_on_post_application_entry(const char* name, REFOnPostApplicationEntryCb cb);

#endif