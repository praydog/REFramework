#include <Windows.h>
#include <reframework/API.hpp>

int pre_start(int argc, void** argv, REFrameworkTypeDefinitionHandle* arg_tys) {
    auto obj = (reframework::API::ManagedObject*)argv[1];
    *obj->get_field<float>("_bodyConstScale") = 1.0f;

    return REFRAMEWORK_HOOK_CALL_ORIGINAL;
}

void post_start(void* ret_val, REFrameworkTypeDefinitionHandle ret_ty) {
}

extern "C" __declspec(dllexport) void reframework_plugin_required_version(REFrameworkPluginVersion* version) {
    version->major = REFRAMEWORK_PLUGIN_VERSION_MAJOR;
    version->minor = REFRAMEWORK_PLUGIN_VERSION_MINOR;
    version->patch = REFRAMEWORK_PLUGIN_VERSION_PATCH;
}

extern "C" __declspec(dllexport) bool reframework_plugin_initialize(const REFrameworkPluginInitializeParam* param) {
    reframework::API::initialize(param);
    reframework::API::get()->tdb()->find_method("snow.player.PlayerWeaponCtrl", "start")->add_hook(pre_start, post_start, false);
    return true;
}
