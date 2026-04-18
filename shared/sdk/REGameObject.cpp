#include "RETypeDB.hpp"

#include "REGameObject.hpp"

#ifdef REFRAMEWORK_UNIVERSAL
#include "GameIdentity.hpp"

RETransform* REGameObject::get_transform() const {
    static const auto offset = []() -> uintptr_t {
        const auto& gi = sdk::GameIdentity::get();
        // SF6 and RE4 have REGameObject::transform at 0x20 (extra padding before transform).
        // All other games have it at 0x18.
        if (gi.is_sf6() || gi.is_re4()) return 0x20;
        return 0x18;
    }();
    return *(RETransform**)((uintptr_t)this + offset);
}
#endif

namespace utility::re_game_object {
std::string get_name(REGameObject* obj) {
    if (obj == nullptr) {
        return {};
    }

    // Only doing this on newer versions where we know it works
    // Haven't tested it on older versions so just to be safe.
#if TDB_VER >= 71
    static const auto game_object_t = sdk::find_type_definition("via.GameObject");
    static const auto get_name_fn = game_object_t != nullptr ? game_object_t->get_method("get_Name") : nullptr;

    if (get_name_fn != nullptr) {
        auto str = get_name_fn->call<::SystemString*>(sdk::get_thread_context(), obj);

        if (str != nullptr) {
            return utility::re_string::get_string(str);
        }
    }
#endif

    // We rely on the reflected function first because
    // this offset might change between versions.
    return utility::re_string::get_string(obj->name);
}
}