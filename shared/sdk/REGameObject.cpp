#include "RETypeDB.hpp"

#include "REGameObject.hpp"

#include "GameIdentity.hpp"

static uintptr_t go_transform_offset() {
    static const auto offset = []() -> uintptr_t {
        const auto& gi = sdk::GameIdentity::get();
        // SF6 and RE4 have REGameObject::transform at 0x20 (extra padding before transform).
        // All other games have it at 0x18.
        if (gi.is_sf6() || gi.is_re4()) return 0x20;
        return 0x18;
    }();
    return offset;
}

static uintptr_t go_bool_base_offset() {
    static const auto offset = []() -> uintptr_t {
        const auto& gi = sdk::GameIdentity::get();
        // RE8/RE2/RE3/DMC5 have 2 bytes of pad at 0x10 before the bools.
        // All other games start bools at 0x10 directly.
        if (gi.is_re8() || gi.is_re2() || gi.is_re3() || gi.is_dmc5()) return 0x12;
        return 0x10;
    }();
    return offset;
}

RETransform* REGameObject::get_transform() const {
    return *(RETransform**)((uintptr_t)this + go_transform_offset());
}

bool REGameObject::get_shouldUpdate() const {
    return *(bool*)((uintptr_t)this + go_bool_base_offset() + 0);
}

bool REGameObject::get_shouldDraw() const {
    return *(bool*)((uintptr_t)this + go_bool_base_offset() + 1);
}

void REGameObject::set_shouldDraw(bool v) {
    *(bool*)((uintptr_t)this + go_bool_base_offset() + 1) = v;
}

void REGameObject::set_shouldUpdate(bool v) {
    *(bool*)((uintptr_t)this + go_bool_base_offset() + 0) = v;
}

REFolder* REGameObject::get_folder() const {
    return *(REFolder**)((uintptr_t)this + go_transform_offset() + sizeof(void*));
}

uintptr_t REGameObject::offset_of_transform() { return go_transform_offset(); }
uintptr_t REGameObject::offset_of_folder()    { return go_transform_offset() + sizeof(void*); }

SystemString* REGameObject::get_name_field() const {
    // name is 2 pointers after transform (transform, folder, name)
    return *(SystemString**)((uintptr_t)this + go_transform_offset() + sizeof(void*) * 2);
}

std::string REGameObject::get_name() const {
    if (this == nullptr) {
        return {};
    }

    // Only doing this on newer versions where we know it works
    // Haven't tested it on older versions so just to be safe.
    static const auto game_object_t = sdk::find_type_definition("via.GameObject");
    static const auto get_name_fn = game_object_t != nullptr ? game_object_t->get_method("get_Name") : nullptr;

    if (get_name_fn != nullptr) {
        auto str = get_name_fn->call<::SystemString*>(sdk::get_thread_context(), const_cast<REGameObject*>(this));

        if (str != nullptr) {
            return utility::re_string::get_string(str);
        }
    }

    // We rely on the reflected function first because
    // this offset might change between versions.
    return utility::re_string::get_string(get_name_field());
}