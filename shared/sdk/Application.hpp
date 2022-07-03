#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

namespace sdk {
struct RETypeDefinition;

struct Application {
    struct Function {
        void* entry; // 0 
        void (*func)(void* entry); // 0x8
        void* unk; // 0x10
        const char* description; // 0x18
        uint16_t priority; // 0x20 (via.ModuleEntry enum)
        uint16_t type; // 0x22
        uint8_t pad[0xD0 - 0x24];
    };

    static RETypeDefinition* get_type();
    static Application* get();

    // Use via.ModuleEntry enum to figure out the function index
    Function* get_functions();
    Function* get_function(uint16_t index);
    Function* get_function(std::string_view name);

    std::vector<Function*> generate_chain(std::string_view start_name, std::string_view end_name);

    float get_delta_time();
    
    static float get_max_fps();
    static void set_max_fps(float max_fps);
    static void set_global_speed(float speed);
    static float get_global_speed();
};
}