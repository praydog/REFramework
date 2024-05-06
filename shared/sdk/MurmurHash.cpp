#include <utility/String.hpp>

#include "RETypeDB.hpp"
#include "RETypeDefinition.hpp"

#include "MurmurHash.hpp"

namespace sdk::murmur_hash {
sdk::RETypeDefinition* type() {
    static auto t = sdk::find_type_definition("via.murmur_hash");
    return t;
}

uint32_t calc32(std::wstring_view str) {
    static auto calc_method = type()->get_method("calc32");

    return calc_method->call<uint32_t>(sdk::get_thread_context(), sdk::VM::create_managed_string(str));
}

uint32_t calc32(std::string_view str) {
    return calc32(utility::widen(str));
}

uint32_t calc32_as_utf8(std::string_view str) {
    static auto calc_method = type()->get_method("calc32AsUTF8");

    return calc_method->call<uint32_t>(sdk::get_thread_context(), sdk::VM::create_managed_string(utility::widen(str)), str.length());
}
}
