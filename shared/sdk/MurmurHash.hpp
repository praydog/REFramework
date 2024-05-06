#pragma once

#include <cstdint>
#include <string_view>

// via.murmur_hash internally
namespace sdk {
struct RETypeDefinition;

namespace murmur_hash {
sdk::RETypeDefinition* type();
uint32_t calc32(std::wstring_view str);
uint32_t calc32(std::string_view str);
uint32_t calc32_as_utf8(std::string_view str);
}
}