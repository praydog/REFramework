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

REFRAMEWORK_API bool reframework_is_ready();
REFRAMEWORK_API bool reframework_on_initialized(REFInitializedCb cb);
REFRAMEWORK_API bool reframework_on_lua_state_created(REFLuaStateCreatedCb cb);
REFRAMEWORK_API bool reframework_on_lua_state_destroyed(REFLuaStateDestroyedCb cb);
REFRAMEWORK_API bool reframework_on_frame(REFOnFrameCb cb);

#endif