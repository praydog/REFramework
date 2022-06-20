#include <spdlog/spdlog.h>

#include "RETypeDB.hpp"
#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "Application.hpp"

namespace sdk {
RETypeDefinition* Application::get_type() {
    return sdk::find_type_definition("via.Application");
}

Application* Application::get() {
    const auto application_type = Application::get_type();

    if (application_type == nullptr) {
        spdlog::error("Cannot find via.Application");
        return nullptr;
    }

    return (sdk::Application*)application_type->get_instance();
}

Application::Function* Application::get_functions() {
    static int32_t functions_offset = -1;

    if (functions_offset == -1) {
        const auto mod = utility::get_executable();

        auto ref = utility::scan(mod, "44 8B ? ? ? 00 00 4C 8D ? ? ? ? 00 41");

        if (!ref) {
            spdlog::error("Cannot find Application::functions offset.");
            return nullptr;
        }

        functions_offset = *(int32_t*)(*ref + 10);

        spdlog::info("Application::functions offset: {:x}", functions_offset);
    }

    return (Application::Function*)((uintptr_t)this + functions_offset);
}

Application::Function* Application::get_function(uint16_t index) {
    auto functions = get_functions();

    if (functions == nullptr) {
        return nullptr;
    }

    for (auto i = 0; i < 1024; ++i) {
        if (functions[i].priority == index) {
            return &functions[i];
        }
    }

    return nullptr;
}

Application::Function* Application::get_function(std::string_view name) {
    auto functions = get_functions();

    if (functions == nullptr) {
        return nullptr;
    }

    const auto module_entry_enum = sdk::find_type_definition("via.ModuleEntry");
    /*const auto function_index = sdk::get_object_field<uint16_t>(nullptr, module_entry_enum, name, true);

    if (function_index == nullptr) {
        return nullptr;
    }*/

    for (auto i = 0; i < 1024; ++i) {
        //if (functions[i].priority == *function_index) {
        if (functions[i].description == name) {
            return &functions[i];
        }
    }

    return nullptr;
}

std::vector<Application::Function*> Application::generate_chain(std::string_view start_name, std::string_view end_name) {
    std::vector<Function*> chain{};
    
    const auto module_entry_enum = sdk::find_type_definition("via.ModuleEntry");
    //const auto start_index = sdk::get_object_field<uint16_t>(nullptr, module_entry_enum, start_name, true);
    //const auto end_index = sdk::get_object_field<uint16_t>(nullptr, module_entry_enum, end_name, true);

    const auto start_index = get_function(start_name)->priority;
    const auto end_index = get_function(end_name)->priority;

    /*if (start_index == nullptr || end_index == nullptr) {
        return chain;
    }*/

    for (auto i = start_index; i <= end_index; ++i) {
        const auto function = get_function(i);

        if (function == nullptr || function->func == nullptr) {
            continue;
        }

        chain.push_back(function);
    }

    return chain;
}

float Application::get_delta_time() {
    static const auto application_type = Application::get_type();

    if (application_type == nullptr) {
        spdlog::error("Cannot find via.Application");
        return 0.0f;
    }

    return sdk::call_native_func_easy<float>(this, application_type, "get_DeltaTime");
}

float Application::get_max_fps() {
    static const auto application_type = Application::get_type();

    if (application_type == nullptr) {
        spdlog::error("Cannot find via.Application");
        return 0.0f;
    }

    static auto get_max_fps_method = application_type->get_method("get_MaxFps");
    return get_max_fps_method->call<float>(sdk::get_thread_context());
}

void Application::set_max_fps(float max_fps) {
    static const auto application_type = Application::get_type();

    if (application_type == nullptr) {
        spdlog::error("Cannot find via.Application");
        return;
    }

    static auto set_max_fps_method = application_type->get_method("set_MaxFps");
    set_max_fps_method->call<void*>(sdk::get_thread_context(), max_fps);
}

void Application::set_global_speed(float speed) {
    static const auto application_type = Application::get_type();

    if (application_type == nullptr) {
        spdlog::error("Cannot find via.Application");
        return;
    }

    static auto set_global_speed_method = application_type->get_method("set_GlobalSpeed");
    set_global_speed_method->call<void*>(sdk::get_thread_context(), speed);
}
}