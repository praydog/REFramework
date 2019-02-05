#pragma once

#include "ReClass.hpp"

namespace utility::REManagedObject {
    static bool isA(::REManagedObject* object, std::string_view name) {
        if (object == nullptr) {
            return false;
        }

        auto info = object->info;

        if (info == nullptr) {
            return false;
        }

        auto classInfo = info->classInfo;

        if (classInfo == nullptr) {
            return false;
        }

        for (auto t = classInfo->type; t != nullptr && t->name != nullptr; t = t->super) {
            if (name == t->name) {
                return true;
            }
        }

        return false;
    }
}