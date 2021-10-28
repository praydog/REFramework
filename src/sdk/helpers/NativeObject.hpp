#pragma once

#include <string>

namespace sdk{
struct RETypeDefinition;

namespace helpers {
struct NativeObject {
    NativeObject(const std::string& n) 
        : name{n}
    {

    }

    bool update();

    void* object{ nullptr };
    sdk::RETypeDefinition* t{ nullptr };

    std::string name{};
};
}
}