#pragma once

#include "ReClass.hpp"

namespace utility::RopewaySweetLightManager {
    static RopewaySweetLightManagerContext* getContext(::RopewaySweetLightManager* mgr, int32_t index) {
        if (mgr == nullptr) {
            return nullptr;
        }

        auto contexts = mgr->contexts;

        if (contexts == nullptr) {
            return nullptr;
        }

        if (index < 0 || index >= contexts->numElements) {
            return nullptr;
        }

        return contexts->data[index];
    }
}