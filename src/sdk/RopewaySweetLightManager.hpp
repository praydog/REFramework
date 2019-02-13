#pragma once

#include "ReClass.hpp"

namespace utility::RopewaySweetLightManager {
    static RopewaySweetLightManagerContext* getContext(::RopewaySweetLightManager* mgr, int32_t index) {
        auto contexts = mgr->contexts;

        if (contexts == nullptr) {
            return nullptr;
        }

        if (mgr == nullptr || index < 0 || index >= contexts->numElements) {
            return nullptr;
        }

        return contexts->data[index];
    }
}