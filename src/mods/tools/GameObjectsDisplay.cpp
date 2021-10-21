#include "REFramework.hpp"
#include "sdk/SceneManager.hpp"
#include "sdk/RETypeDB.hpp"
#include "sdk/REManagedObject.hpp"

#include "GameObjectsDisplay.hpp"

namespace detail {
struct Invocation {
    void* object_ptr;
    void* method_ptr;
    void* unk;
};

struct Predicate : public REManagedObject {
    uint32_t num_invocations{0};
    std::array<Invocation, 10> invocations;
};
}

std::optional<std::string> GameObjectsDisplay::on_initialize() {

    // OK
    return Mod::on_initialize();
}

void GameObjectsDisplay::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }
}

void GameObjectsDisplay::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}


void GameObjectsDisplay::on_draw_dev_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_enabled->draw("Enabled") && !m_enabled->value()) {
        // todo
    }
}

void GameObjectsDisplay::on_frame() {
    if (!m_enabled->value()) {
        return;
    }

    auto scene = sdk::get_current_scene();

    if (scene == nullptr) {
        return;
    }

    auto context = sdk::get_thread_context();

    static auto scene_def = sdk::RETypeDB::get()->find_type("via.Scene");
    auto first_transform = sdk::call_object_func<RETransform*>(scene, scene_def, "get_FirstTransform", context, scene);

    if (first_transform == nullptr) {
        return;
    }

    static auto transform_def = utility::re_managed_object::get_type_definition(first_transform);
    static auto next_transform_method = transform_def->get_method("get_Next");
    static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    static auto get_position_method = transform_def->get_method("get_Position");
    static auto get_axisz_method = transform_def->get_method("get_AxisZ");

    auto math = sdk::get_native_singleton("via.math");
    auto math_t = sdk::RETypeDB::get()->find_type("via.math");

    auto camera = sdk::get_primary_camera();

    if (camera == nullptr) {
        return;
    }

    auto main_view = sdk::get_main_view();

    if (main_view == nullptr) {
        return;
    }

    auto camera_gameobject = get_gameobject_method->call<REGameObject*>(context, camera);
    auto camera_transform = camera_gameobject->transform;

    Vector4f camera_origin{};
    get_position_method->call<void*>(&camera_origin, context, camera_transform);

    Vector4f camera_forward{};
    get_axisz_method->call<void*>(&camera_forward, context, camera_transform);

    // Translate 2d position to 3d position (screen to world)
    Matrix4x4f proj{}, view{};
    float screen_size[2]{};
    sdk::call_object_func<void*>(camera, "get_ProjectionMatrix", &proj, context, camera);
    sdk::call_object_func<void*>(camera, "get_ViewMatrix", &view, context, camera);
    sdk::call_object_func<void*>(main_view, "get_Size", &screen_size, context, main_view);

    static auto world_to_screen_methods = math_t->get_methods("worldPos2ScreenPos"); // there are 2 of them.

    Vector4f pos{};
    Vector4f screen_pos{};

    auto draw_list = ImGui::GetBackgroundDrawList();

    for (auto transform = first_transform; 
        transform != nullptr; 
        transform = next_transform_method->call<RETransform*>(context, transform)) 
    {
        auto owner = get_gameobject_method->call<REGameObject*>(context, transform);

        if (owner == nullptr) {
            continue;
        }

        auto owner_name = utility::re_string::get_string(owner->name);

        if (owner_name.empty()) {
            continue;
        }

        get_position_method->call<void*>(&pos, context, transform);

        const auto delta = pos - camera_origin;

        // behind camera
        if (glm::dot(delta, -camera_forward) <= 0.0f) {
            continue;
        }

        world_to_screen_methods[1]->call<void*>(&screen_pos, context, &pos, &view, &proj, &screen_size);
        draw_list->AddText(ImVec2(screen_pos.x, screen_pos.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), owner_name.c_str());
    }
}

