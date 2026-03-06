#pragma once
// Runtime dispatch for regenny struct field offsets that vary per game.
// Used under REFRAMEWORK_UNIVERSAL to replace compile-time header selection.

#include <cstdint>
#include <cstddef>
#include <sdk/GameIdentity.hpp>

namespace sdk::via {

// ──────────────────────────────────────────────
// via::Window field offsets
// ──────────────────────────────────────────────
struct WindowOffsets {
    size_t width;
    size_t height;
    size_t cursor_pos;
    size_t show_cursor;
    size_t borderless_size;  // 0 if not present (RE7)
};

inline const WindowOffsets& window_offsets() {
    static const WindowOffsets offsets = []() -> WindowOffsets {
        switch (GameIdentity::get().game()) {
        case GameID::RE7:
            return { 0x40, 0x44, 0x48, 0x50, 0 };
        case GameID::RE3:
        case GameID::DMC5:
            return { 0x48, 0x4c, 0x50, 0x58, 0x88 };
        case GameID::RE2:
        case GameID::MHRISE:
        case GameID::SF6:
            return { 0x58, 0x5c, 0x60, 0x68, 0x98 };
        case GameID::DD2:
            return { 0x60, 0x64, 0x68, 0x70, 0xa0 };
        case GameID::RE8:
            return { 0x70, 0x74, 0x78, 0x80, 0xb0 };
        case GameID::RE4:
            return { 0x78, 0x7c, 0x80, 0x88, 0xb8 };
        default: // RE9, MHWILDS, MHSTORIES3, PRAGMATA — use re9 layout
            return { 0x60, 0x64, 0x68, 0x70, 0xa8 };
        }
    }();
    return offsets;
}

// Typed accessors for via::Window — take opaque void* since the compiled
// struct layout does not match the runtime game.
inline uint32_t& window_width(void* w)  { return *(uint32_t*)((uintptr_t)w + window_offsets().width); }
inline uint32_t& window_height(void* w) { return *(uint32_t*)((uintptr_t)w + window_offsets().height); }

// borderless_size is a via::Size { float w; float h; } — return float refs.
inline float& window_borderless_w(void* w) { return *(float*)((uintptr_t)w + window_offsets().borderless_size); }
inline float& window_borderless_h(void* w) { return *(float*)((uintptr_t)w + window_offsets().borderless_size + 4); }
inline bool   window_has_borderless(void* w) { (void)w; return window_offsets().borderless_size != 0; }

// ──────────────────────────────────────────────
// via::SceneView field offsets
// ──────────────────────────────────────────────
struct SceneViewOffsets {
    size_t window;           // pointer to via::Window
    size_t scene;            // pointer to via::Scene
    size_t size;             // via::Size {float w, float h}
    size_t custom_display_size; // via::Size
    size_t present_rect;     // via::Size
    size_t display_type;     // via::DisplayType (int32)
};

inline const SceneViewOffsets& scene_view_offsets() {
    static const SceneViewOffsets offsets = []() -> SceneViewOffsets {
        switch (GameIdentity::get().game()) {
        case GameID::RE7:
            return { 0x20, 0x28, 0x58, 0x60, 0x68, 0x74 };
        case GameID::RE3:
        case GameID::DMC5:
            return { 0x10, 0x18, 0x48, 0x50, 0x58, 0x74 };
        default: // RE2, RE4, RE8, RE9, MHRISE, SF6, DD2, MHWILDS, etc.
            return { 0x10, 0x18, 0x30, 0x38, 0x40, 0x64 };
        }
    }();
    return offsets;
}

// Typed accessors for via::SceneView — take opaque void*.
inline void*  sv_window(void* sv)        { return *(void**)((uintptr_t)sv + scene_view_offsets().window); }
inline void*  sv_scene(void* sv)         { return *(void**)((uintptr_t)sv + scene_view_offsets().scene); }
inline float& sv_size_w(void* sv)        { return *(float*)((uintptr_t)sv + scene_view_offsets().size); }
inline float& sv_size_h(void* sv)        { return *(float*)((uintptr_t)sv + scene_view_offsets().size + 4); }
inline float& sv_custom_disp_w(void* sv) { return *(float*)((uintptr_t)sv + scene_view_offsets().custom_display_size); }
inline float& sv_custom_disp_h(void* sv) { return *(float*)((uintptr_t)sv + scene_view_offsets().custom_display_size + 4); }
inline float& sv_present_w(void* sv)     { return *(float*)((uintptr_t)sv + scene_view_offsets().present_rect); }
inline float& sv_present_h(void* sv)     { return *(float*)((uintptr_t)sv + scene_view_offsets().present_rect + 4); }
inline int32_t& sv_display_type(void* sv){ return *(int32_t*)((uintptr_t)sv + scene_view_offsets().display_type); }

} // namespace sdk::via
