#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

#include "TDBVer.hpp"

namespace sdk {
struct RETypeDefinition;

struct Application {
    struct Function {
        void* entry; // 0x00
        void (*func)(void* entry); // 0x08
    
#ifdef REFRAMEWORK_UNIVERSAL
        // TDB < 74: extra void* unk at 0x10 shifts description to 0x18, priority to 0x20, type to 0x22. Stride = 0xD0.
        // TDB >= 74: description at 0x10, priority at 0x18, type to 0x1A. Stride = 0xC8.
        // Compiled with tdb84 layout; runtime accessors handle the offset shift.
    private:
        const char* _description; // 0x10 placeholder (wrong offset for TDB < 74)
        uint16_t _priority;       // 0x18 placeholder
        uint16_t _type;           // 0x1A placeholder
        uint8_t _pad[0xC8 - 0x1C];
    public:
        const char* get_description() const;
        uint16_t get_priority() const;
        uint16_t get_type_val() const;
    };
    static_assert(sizeof(Function) == 0xC8, "Function has wrong size");

    static size_t get_function_stride();
    static Function* get_function_at(Function* base, size_t index);
#else
#if TDB_VER < 74
        void* unk; // 0x10
#endif

        const char* description; // 0x18 or 0x10
        uint16_t priority; // 0x20 or 0x18 (via.ModuleEntry enum)
        uint16_t type; // 0x22 or 0x1A

#if TDB_VER >= 74
        uint8_t pad[0xC8 - 0x1C];
#else
        uint8_t pad[0xD0 - 0x24];
#endif

        const char* get_description() const { return description; }
        uint16_t get_priority() const { return priority; }
        uint16_t get_type_val() const { return type; }
    };

#if TDB_VER >= 74
    static_assert(sizeof(Function) == 0xC8, "Function has wrong size");
#elif TDB_VER < 74
    static_assert(sizeof(Function) == 0xD0, "Function has wrong size");
#endif

    static size_t get_function_stride() { return sizeof(Function); }
    static Function* get_function_at(Function* base, size_t index) {
        return reinterpret_cast<Function*>(reinterpret_cast<uintptr_t>(base) + index * sizeof(Function));
    }
#endif  // REFRAMEWORK_UNIVERSAL

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