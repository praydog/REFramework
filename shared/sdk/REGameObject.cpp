#include "RETypeDB.hpp"

#include "REGameObject.hpp"

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