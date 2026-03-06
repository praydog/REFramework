#pragma once
// Runtime dispatch for regenny via::Window / via::SceneView structs.
// Each game's regenny output is included inside a distinct outer namespace so
// that all layouts coexist.  Accessor functions cast to the right type at
// runtime based on GameIdentity.
//
// When Regenny regenerates a header, offsets update automatically — no manual
// offset tables to maintain.
//
// NOTE: The caller must include the "default" re9 regenny headers in the
// global scope BEFORE including this header (Graphics.cpp / VR.cpp already
// do this).  We skip re9 here because #pragma once would block re-inclusion.

#include <cstdint>
#include <sdk/GameIdentity.hpp>

// ── Namespaced includes for each game variant (except re9, see note) ────────

namespace ns_re7 {
#include "sdk/regenny/re7/via/Window.hpp"
#include "sdk/regenny/re7/via/SceneView.hpp"
}

namespace ns_re3 {
#include "sdk/regenny/re3/via/Window.hpp"
#include "sdk/regenny/re3/via/SceneView.hpp"
}

namespace ns_re2 {
#include "sdk/regenny/re2_tdb70/via/Window.hpp"
#include "sdk/regenny/re2_tdb70/via/SceneView.hpp"
}

namespace ns_mhrise {
#include "sdk/regenny/mhrise_tdb71/via/Window.hpp"
#include "sdk/regenny/mhrise_tdb71/via/SceneView.hpp"
}

namespace ns_re8 {
#include "sdk/regenny/re8/via/Window.hpp"
#include "sdk/regenny/re8/via/SceneView.hpp"
}

namespace ns_re4 {
#include "sdk/regenny/re4/via/Window.hpp"
#include "sdk/regenny/re4/via/SceneView.hpp"
}

namespace ns_sf6 {
#include "sdk/regenny/sf6/via/Window.hpp"
#include "sdk/regenny/sf6/via/SceneView.hpp"
}

namespace ns_dd2 {
#include "sdk/regenny/dd2/via/Window.hpp"
#include "sdk/regenny/dd2/via/SceneView.hpp"
}

// re9 headers are already included globally by the caller — use ::regenny::via.

// ── Dispatch helpers ────────────────────────────────────────────────────────

namespace sdk::via {

// Convenience aliases for the namespaced Window types.
namespace W {
    using re7    = ns_re7::regenny::via::Window;
    using re3    = ns_re3::regenny::via::Window;
    using re2    = ns_re2::regenny::via::Window;
    using mhrise = ns_mhrise::regenny::via::Window;
    using re8    = ns_re8::regenny::via::Window;
    using re4    = ns_re4::regenny::via::Window;
    using sf6    = ns_sf6::regenny::via::Window;
    using dd2    = ns_dd2::regenny::via::Window;
    using re9    = ::regenny::via::Window;  // global — already included
}

namespace SV {
    using re7    = ns_re7::regenny::via::SceneView;
    using re3    = ns_re3::regenny::via::SceneView;
    using re2    = ns_re2::regenny::via::SceneView;
    using mhrise = ns_mhrise::regenny::via::SceneView;
    using re8    = ns_re8::regenny::via::SceneView;
    using re4    = ns_re4::regenny::via::SceneView;
    using sf6    = ns_sf6::regenny::via::SceneView;
    using dd2    = ns_dd2::regenny::via::SceneView;
    using re9    = ::regenny::via::SceneView;  // global
}

// ── via::Window accessors ───────────────────────────────────────────────────

// Macro: dispatches to the correct namespaced Window struct and returns a
// reference to the named field.
#define VIA_WIN_FIELD(w, field) \
    switch (sdk::GameIdentity::get().game()) { \
    case sdk::GameID::RE7:                                      \
    case sdk::GameID::RE2:     return ((W::re2*)(w))->field;    \
    case sdk::GameID::RE3:                                      \
    case sdk::GameID::DMC5:    return ((W::re3*)(w))->field;    \
    case sdk::GameID::MHRISE:  return ((W::mhrise*)(w))->field; \
    case sdk::GameID::RE8:     return ((W::re8*)(w))->field;    \
    case sdk::GameID::RE4:     return ((W::re4*)(w))->field;    \
    case sdk::GameID::SF6:     return ((W::sf6*)(w))->field;    \
    case sdk::GameID::DD2:     return ((W::dd2*)(w))->field;    \
    default:                   return ((W::re9*)(w))->field;    \
    }

inline uint32_t& window_width(void* w)  { VIA_WIN_FIELD(w, width); }
inline uint32_t& window_height(void* w) { VIA_WIN_FIELD(w, height); }

// borderless_size is a via::Size { float w; float h; }.
// Modern RE7 (TDB 70) shares the RE2 Window layout.
#define VIA_WIN_BORDERLESS(w, sub) \
    switch (sdk::GameIdentity::get().game()) { \
    case sdk::GameID::RE7:                                                     \
    case sdk::GameID::RE2:     return ((W::re2*)(w))->borderless_size.sub;     \
    case sdk::GameID::RE3:                                                     \
    case sdk::GameID::DMC5:    return ((W::re3*)(w))->borderless_size.sub;     \
    case sdk::GameID::MHRISE:  return ((W::mhrise*)(w))->borderless_size.sub;  \
    case sdk::GameID::RE8:     return ((W::re8*)(w))->borderless_size.sub;     \
    case sdk::GameID::RE4:     return ((W::re4*)(w))->borderless_size.sub;     \
    case sdk::GameID::SF6:     return ((W::sf6*)(w))->borderless_size.sub;     \
    case sdk::GameID::DD2:     return ((W::dd2*)(w))->borderless_size.sub;     \
    default:                   return ((W::re9*)(w))->borderless_size.sub;     \
    }

inline float& window_borderless_w(void* w) { VIA_WIN_BORDERLESS(w, w); }
inline float& window_borderless_h(void* w) { VIA_WIN_BORDERLESS(w, h); }

#undef VIA_WIN_FIELD
#undef VIA_WIN_BORDERLESS

// ── via::SceneView accessors ────────────────────────────────────────────────

// Returns the via::Window* stored in the SceneView.
inline void* sv_window(void* sv) {
    switch (sdk::GameIdentity::get().game()) {
    case sdk::GameID::RE7:
    case sdk::GameID::RE2:     return ((SV::re2*)sv)->window;
    case sdk::GameID::RE3:
    case sdk::GameID::DMC5:    return ((SV::re3*)sv)->window;
    case sdk::GameID::MHRISE:  return ((SV::mhrise*)sv)->window;
    case sdk::GameID::RE8:     return ((SV::re8*)sv)->window;
    case sdk::GameID::RE4:     return ((SV::re4*)sv)->window;
    case sdk::GameID::SF6:     return ((SV::sf6*)sv)->window;
    case sdk::GameID::DD2:     return ((SV::dd2*)sv)->window;
    default:                   return ((SV::re9*)sv)->window;
    }
}

// size is via::Size { float w; float h; }.
// All namespaced Size types are layout-compatible, so we return float&.
#define VIA_SV_SIZE(sv, sub) \
    switch (sdk::GameIdentity::get().game()) { \
    case sdk::GameID::RE7:                                           \
    case sdk::GameID::RE2:     return ((SV::re2*)(sv))->size.sub;    \
    case sdk::GameID::RE3:                                           \
    case sdk::GameID::DMC5:    return ((SV::re3*)(sv))->size.sub;    \
    case sdk::GameID::MHRISE:  return ((SV::mhrise*)(sv))->size.sub; \
    case sdk::GameID::RE8:     return ((SV::re8*)(sv))->size.sub;    \
    case sdk::GameID::RE4:     return ((SV::re4*)(sv))->size.sub;    \
    case sdk::GameID::SF6:     return ((SV::sf6*)(sv))->size.sub;    \
    case sdk::GameID::DD2:     return ((SV::dd2*)(sv))->size.sub;    \
    default:                   return ((SV::re9*)(sv))->size.sub;    \
    }

inline float& sv_size_w(void* sv) { VIA_SV_SIZE(sv, w); }
inline float& sv_size_h(void* sv) { VIA_SV_SIZE(sv, h); }

#undef VIA_SV_SIZE

// display_type — each namespace has its own DisplayType enum, but they're all
// int32 under the hood.  Return int32_t& so all branches agree on type.
inline int32_t& sv_display_type(void* sv) {
    switch (sdk::GameIdentity::get().game()) {
    case sdk::GameID::RE7:
    case sdk::GameID::RE2:     return *(int32_t*)&((SV::re2*)sv)->display_type;
    case sdk::GameID::RE3:
    case sdk::GameID::DMC5:    return *(int32_t*)&((SV::re3*)sv)->display_type;
    case sdk::GameID::MHRISE:  return *(int32_t*)&((SV::mhrise*)sv)->display_type;
    case sdk::GameID::RE8:     return *(int32_t*)&((SV::re8*)sv)->display_type;
    case sdk::GameID::RE4:     return *(int32_t*)&((SV::re4*)sv)->display_type;
    case sdk::GameID::SF6:     return *(int32_t*)&((SV::sf6*)sv)->display_type;
    case sdk::GameID::DD2:     return *(int32_t*)&((SV::dd2*)sv)->display_type;
    default:                   return *(int32_t*)&((SV::re9*)sv)->display_type;
    }
}

// Offset of display_type within the SceneView (for the Sunbreak +4 trick).
inline size_t sv_display_type_offset() {
    // Use pointer arithmetic on a null pointer — no UB, just offset calc.
    #define SV_DT_OFF(T) ((size_t)&((T*)nullptr)->display_type)
    switch (sdk::GameIdentity::get().game()) {
    case sdk::GameID::RE7:
    case sdk::GameID::RE2:     return SV_DT_OFF(SV::re2);
    case sdk::GameID::RE3:
    case sdk::GameID::DMC5:    return SV_DT_OFF(SV::re3);
    default:                   return SV_DT_OFF(SV::re9);
    }
    #undef SV_DT_OFF
}

} // namespace sdk::via
