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
    static auto functions_offset = [this]() -> std::optional<int32_t> {
        spdlog::info("Searching for Application::functions offset...");

        const auto mod = utility::get_executable();
        const auto mod_size = utility::get_module_size(mod);
        const auto mod_end = (uintptr_t)mod + *mod_size - 0x100;

        // For MHRise (game pass only? or TU4)
        for (auto ref = utility::scan(mod, "89 81 ? ? ? ? 48 8B ? 48 81 C1 ? ? ? ?");
            ref;
            ref = utility::scan(*ref + 1, mod_end - (*ref + 1), "89 81 ? ? ? ? 48 8B ? 48 81 C1 ? ? ? ?")) 
        {
            if (!ref) {
                spdlog::info("MHRise pattern not found, skipping...");
                break;
            }

            const auto candidate = *(int32_t*)(*ref + 12) - 8;

            // If the offset is aligned to 8 bytes, it's a valid offset.
            if (((uintptr_t)candidate & (sizeof(void*) - 1)) == 0 && candidate >= 0x400) {
                spdlog::info("Application::functions offset: {:x}", candidate);
                return candidate;
            }

            spdlog::info("Skipping invalid Application::functions offset: {:x}", candidate);
        }

        // For all the other RE Engine games in existence.
        for (auto ref = utility::scan(mod, "44 8B ? ? ? 00 00 4C 8D ? ? ? ? 00 41");
            ref;
            ref = utility::scan(*ref + 1, mod_end - (*ref + 1), "44 8B ? ? ? 00 00 4C 8D ? ? ? ? 00 41")) 
        {
            if (!ref) {
                spdlog::error("Cannot find Application::functions offset.");
                return std::nullopt;
            }

            const auto candidate = *(int32_t*)(*ref + 10);

            // If the offset is aligned to 8 bytes, it's a valid offset.
            if (((uintptr_t)candidate & (sizeof(void*) - 1)) == 0 && candidate >= 0x400) {
                spdlog::info("Application::functions offset: {:x}", candidate);
                return candidate;
            }

            spdlog::info("Skipping invalid Application::functions offset: {:x}", candidate);
        }

        bool found_wait_rendering = false;
        bool found_begin_rendering = false;
        bool found_end_rendering = false;

        const auto module_entry_enum = sdk::find_type_definition("via.ModuleEntry");

        // screw that lets just bruteforce through the Application object looking for huge
        // list of valid pointers within the current module
        for (auto i = 0x100; i < 0x1000; i += sizeof(void*)) try {
            const auto ptr = (Application::Function*)((uintptr_t)this + i);

            if (ptr == nullptr || IsBadReadPtr(ptr, sizeof(Application::Function) * 50)) {
                continue;
            }

            for (auto j = 0; j < 1024; ++j) try {
                const auto& func = ptr[j];

                if (func.description == nullptr || IsBadReadPtr(func.description, 32)) {
                    break;
                }

                if (j == 0 && func.entry == nullptr || IsBadReadPtr(func.entry, sizeof(void*))) {
                    break; // the first one should always be valid.
                }

                const auto name = std::string_view{func.description};

                if (j == 0) {
                    if (module_entry_enum != nullptr) {
                        if (auto f = sdk::get_native_field<uint16_t>(nullptr, module_entry_enum, name, true); f != nullptr) {
                            if (*f != func.priority) {
                                break; // the first one should always be valid.
                            }
                        }
                    } else if (func.priority != 1) {
                        break; // the first one should always be valid.
                    }
                }

                if (name == "WaitRendering") {
                    if (module_entry_enum != nullptr) {
                        if (auto f = sdk::get_native_field<uint16_t>(nullptr, module_entry_enum, "WaitRendering", true); f != nullptr) {
                            if (*f == func.priority) {
                                found_wait_rendering = true;
                            }
                        }
                    } else {
                        found_wait_rendering = true;
                    }
                } else if (name == "BeginRendering") {
                    if (module_entry_enum != nullptr) {
                        if (auto f = sdk::get_native_field<uint16_t>(nullptr, module_entry_enum, "BeginRendering", true); f != nullptr) {
                            if (*f == func.priority) {
                                found_begin_rendering = true;
                            }
                        }
                    } else {
                        found_begin_rendering = true;
                    }
                } else if (name == "EndRendering") {
                    if (module_entry_enum != nullptr) {
                        if (auto f = sdk::get_native_field<uint16_t>(nullptr, module_entry_enum, "EndRendering", true); f != nullptr) {
                            if (*f == func.priority) {
                                found_end_rendering = true;
                            }
                        }
                    } else {
                        found_end_rendering = true;
                    }
                }

                if (found_wait_rendering && found_begin_rendering && found_end_rendering) {
                    spdlog::info("Application::functions offset: {:x}", i);
                    return i;
                }
            } catch (...) {
                continue;
            }
        } catch(...) {
            continue;
        }


        spdlog::error("Cannot find Application::functions offset.");

        return std::nullopt;
    }();

    if (!functions_offset) {
        return nullptr;
    }

    return (Application::Function*)((uintptr_t)this + *functions_offset);
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
        if (functions[i].description != nullptr && functions[i].description == name) {
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

float Application::get_global_speed() {
    static const auto application_type = Application::get_type();

    if (application_type == nullptr) {
        spdlog::error("Cannot find via.Application");
        return 0.0f;
    }

    static auto get_global_speed_method = application_type->get_method("get_GlobalSpeed");
    return get_global_speed_method->call<float>(sdk::get_thread_context());
}
}