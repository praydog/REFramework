#include <spdlog/spdlog.h>

#include "RETypeDB.hpp"
#include "SceneManager.hpp"

namespace sdk {
template<typename T, typename ...Args>
T call_scenemanager_func(std::string_view name, Args... args) {
    auto scene_manager = sdk::get_scene_manager();

    if (scene_manager == nullptr) {
        return nullptr;
    }

    const auto scene_manager_type = sdk::RETypeDB::get()->find_type("via.SceneManager");

    return sdk::call_object_func<T>(scene_manager, scene_manager_type, name, sdk::get_thread_context(), scene_manager, args...);
}

void* get_scene_manager() {
    const auto scene_manager_type = sdk::RETypeDB::get()->find_type("via.SceneManager");

    if (scene_manager_type == nullptr) {
        spdlog::error("Cannot find via.SceneManager");
        return nullptr;
    }

    return scene_manager_type->get_instance();
}

REManagedObject* get_main_view() {
    return call_scenemanager_func<REManagedObject*>("get_MainView");
}

REManagedObject* get_current_scene() {
    return call_scenemanager_func<REManagedObject*>("get_CurrentScene");
}

RECamera* get_primary_camera() {
    auto main_view = get_main_view();

    if (main_view == nullptr) {
        return nullptr;
    }

    return call_object_func<RECamera*>(main_view, "get_PrimaryCamera", sdk::get_thread_context(), main_view);
}

void set_timescale(REManagedObject* scene, float timescale) {
    const auto scene_type =utility::re_managed_object::get_type_definition(scene);
    const auto set_timescale_method = scene_type->get_method("set_TimeScale");

    set_timescale_method->call(sdk::get_thread_context(), scene, timescale);
}

void set_timescale(float timescale) {
    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return;
    }

    set_timescale(scene, timescale);
}

float get_timescale(REManagedObject* scene) {
    const auto scene_type = utility::re_managed_object::get_type_definition(scene);
    const auto set_timescale_method = scene_type->get_method("get_TimeScale");

    return set_timescale_method->call<float>(sdk::get_thread_context(), scene);
}

float get_timescale() {
    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return 1.0f;
    }

    return sdk::get_timescale(scene);
}
} // namespace sdk