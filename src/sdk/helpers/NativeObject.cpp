#include "../RETypeDB.hpp"

#include "NativeObject.hpp"

bool sdk::helpers::NativeObject::update() {
        if (this->object != nullptr && this->t != nullptr) {
        return true;
    }

    this->object = sdk::get_native_singleton(this->name);
    this->t = sdk::RETypeDB::get()->find_type(this->name);

    return this->object != nullptr && this->t != nullptr;
}

