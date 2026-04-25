#pragma once

#include <cstdint>

#ifdef REFRAMEWORK_UNIVERSAL
#include "../GameIdentity.hpp"
#endif

namespace sdk::renderer {
class RenderResource {
public:
    void add_ref();
    void release();

    using ReleaseFn = void(*)(RenderResource* resource);
    static ReleaseFn get_release_fn();

#ifdef REFRAMEWORK_UNIVERSAL
    // sizeof(RenderResource) is fixed at compile time, but the real game-side struct
    // size varies per game/TDB version because of conditional padding. Universal code
    // that uses `sizeof(RenderResource)` to compute member offsets MUST use this helper
    // instead so it dispatches to the correct size at runtime.
    //
    // Layout (matches the #if blocks below):
    //   base (0x10): vtable + ref_count + render_frame
    //   +0x08 _tdb73_padding when: SF6, RE4, TDB >= 73, or TDB <= 67
    //   +0x08 tdb82_padding   when: TDB >= 82
    static inline uintptr_t get_runtime_size() {
        const auto& gi = sdk::GameIdentity::get();
        const auto v = gi.tdb_ver();

        uintptr_t size = 0x10;
        if (gi.is_sf6() || gi.is_re4() || v >= 73 || v <= 67) {
            size += sizeof(void*); // _tdb73_padding
        }
        if (v >= 82) {
            size += sizeof(void*); // tdb82_padding
        }
        return size;
    }
#endif

public:
    void* m_vtable;
    int32_t m_ref_count;
    uint32_t m_render_frame;

#if defined(SF6) || TDB_VER >= 73 || TDB_VER <= 67 || defined (RE4) /* idk why new re4 has this. */
    void* _tdb73_padding; // something added here in Dec 1 2023 update
#endif

    #if TDB_VER >= 82
    void* tdb82_padding; // something added in RE9?
    #endif
};

#if defined(RE9) && !defined(REFRAMEWORK_UNIVERSAL)
    static_assert(sizeof(RenderResource) == 0x20);
#endif
}
