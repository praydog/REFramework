#pragma once

class ScriptState;
struct lua_State;

namespace bindings {
void open_sdk(ScriptState* s);

// Called once per frame: wrappers pushed last frame stop being kept unless they were pushed again this frame.
void rotate_recent_wrappers(lua_State* l);
}
