#pragma once

#include <cstdint>

#include "RENativeArray.hpp"

#include "ReClass.hpp"

#ifdef RE7
#include "sdk/regenny/re7/via/reflection/Property.hpp"
#include "sdk/regenny/re7/via/typeinfo/TypeInfoCLR.hpp"
#endif

#ifdef RE7
// RE7 does not embed the sizes and alignments
// within the deserializer structures,
// so we need to define them here.
// Can be found in future games by pattern scanning
// for the early bytes in the array
namespace sdk {
// Generated with IDA
constexpr std::array<uint64_t, 76> typekind_alignments{
    0x0000000000000000, 0x0000000000000008, 0x0000000000000001, 0x0000000000000010, 
    0x0000000000000008, 0x0000000000000008, 0x0000000000000008, 0x0000000000000001, 
    0x0000000000000001, 0x0000000000000002, 0x0000000000000001, 0x0000000000000001, 
    0x0000000000000002, 0x0000000000000002, 0x0000000000000004, 0x0000000000000004, 
    0x0000000000000008, 0x0000000000000008, 0x0000000000000004, 0x0000000000000008, 
    0x0000000000000008, 0x0000000000000008, 0x0000000000000004, 0x0000000000000004, 
    0x0000000000000004, 0x0000000000000004, 0x0000000000000004, 0x0000000000000004, 
    0x0000000000000004, 0x0000000000000004, 0x0000000000000004, 0x0000000000000004, 
    0x0000000000000004, 0x0000000000000004, 0x0000000000000004, 0x0000000000000004, 
    0x0000000000000002, 0x0000000000000002, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000008, 0x0000000000000004, 0x0000000000000008, 
    0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000004, 0x0000000000000004, 0x0000000000000004, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000010, 0x0000000000000004, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000004, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000008, 0x0000000000000008, 0x0000000000000008
};

constexpr std::array<uint64_t, 76> typekind_sizes{
    0x0000000000000000, 0x0000000000000008, 0x0000000000000000, 0x0000000000000000, 
    0x0000000000000008, 0x0000000000000008, 0x0000000000000008, 0x0000000000000001, 
    0x0000000000000001, 0x0000000000000002, 0x0000000000000001, 0x0000000000000001, 
    0x0000000000000002, 0x0000000000000002, 0x0000000000000004, 0x0000000000000004, 
    0x0000000000000008, 0x0000000000000008, 0x0000000000000004, 0x0000000000000008, 
    0x0000000000000020, 0x0000000000000020, 0x0000000000000004, 0x0000000000000008, 
    0x000000000000000C, 0x0000000000000010, 0x0000000000000008, 0x000000000000000C, 
    0x0000000000000010, 0x0000000000000008, 0x000000000000000C, 0x0000000000000010, 
    0x0000000000000024, 0x0000000000000030, 0x0000000000000030, 0x0000000000000030, 
    0x0000000000000004, 0x0000000000000008, 0x0000000000000030, 0x0000000000000040, 
    0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000010, 0x0000000000000010, 0x0000000000000004, 0x0000000000000008, 
    0x0000000000000020, 0x0000000000000030, 0x0000000000000020, 0x0000000000000020, 
    0x0000000000000020, 0x0000000000000050, 0x0000000000000010, 0x0000000000000010, 
    0x0000000000000008, 0x0000000000000008, 0x0000000000000008, 0x0000000000000020, 
    0x0000000000000020, 0x0000000000000020, 0x0000000000000008, 0x0000000000000010, 
    0x0000000000000030, 0x0000000000000030, 0x0000000000000020, 0x0000000000000050, 
    0x0000000000000020, 0x0000000000000010, 0x0000000000000020, 0x0000000000000070, 
    0x0000000000000010, 0x0000000000000020, 0x0000000000000010, 0x0000000000000008
};
}
#endif

// Structures for CLR types
// Same structure across all the games so we don't care about the ReClass header
namespace sdk {
struct RETypeDefinition;

struct DeserializeSequence {
#ifndef RE7
    uint32_t code : 8;
    uint32_t size : 8;
    uint32_t align : 8;
    uint32_t depth : 6;
    uint32_t is_array_ : 1;
    uint32_t is_static_ : 1;
#else
    uint16_t code;
    uint16_t depth;
#endif
    
    uint32_t offset;

#ifndef RE7
    sdk::RETypeDefinition* native_type;
#else
    regenny::via::reflection::Property* prop; // aka REVariableDescriptor in ReClass
#endif

// Helper functions
public:
    sdk::RETypeDefinition* get_native_type() const {
#ifdef RE7
        if (prop != nullptr && prop->vm_field != nullptr && prop->vm_field->type != nullptr && prop->vm_field->type->tdb_type != nullptr) {
            return (sdk::RETypeDefinition*)prop->vm_field->type->tdb_type;
        }

        return nullptr;
#else
        return native_type;
#endif
    }

    uint16_t get_code() const {
        return code;
    }

    uint16_t get_size() const {
#ifdef RE7
        return (uint16_t)typekind_sizes[code];
#else
        return depth;
#endif
    }

    uint16_t get_align() const {
#ifdef RE7
        return (uint16_t)typekind_alignments[code];
#else
        return align;
#endif
    }

    uint16_t get_depth() const {
        return depth;
    }

    uint16_t get_offset() const {
        return offset;
    }

    bool is_array() const {
#ifdef RE7
        if (prop == nullptr) {
            return false;
        }

        // this is wack af good thing its only in re7
        return prop->getter_array != nullptr && prop->getter_array2 != nullptr;
#else
        return is_array_;
#endif
    }

    bool is_static() const {
#ifdef RE7
        return false; // FIGURE IT OUT LATER
#else
        return is_static_;
#endif
    }
};

struct RETypeCLR : public ::REType {
    sdk::NativeArray<sdk::DeserializeSequence> deserializers;

#ifndef RE7
    class ::REType* native_type;
    char* name2;
#else
    // dunno lol
#endif
};
} // namespace sdk