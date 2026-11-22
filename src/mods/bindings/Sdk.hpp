#pragma once

class ScriptState;
struct lua_State;

namespace bindings {
void open_sdk(ScriptState* s);

// Called once per frame, after the frame's GC step: releases the engine references of objects whose wrappers the
// collector freed a few frames ago and that no script pushed again since.
void scan_wrapper_references(lua_State* l);
}
