#pragma once

class ScriptState;

namespace bindings {
void open_imgui(ScriptState* s);
}

namespace api::imgui {
void cleanup();
}

namespace api::imnodes {
// pop the active nodes/editors/attributes etc so we dont crash.
void cleanup();
}