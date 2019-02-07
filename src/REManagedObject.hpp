#pragma once

#include "Address.hpp"
#include "ReClass.hpp"

namespace utility::REManagedObject {
    static bool isManagedObject(Address address) {
        if (address == nullptr) {
            return false;
        }

        if (IsBadReadPtr(address.ptr(), sizeof(void*))) {
            return false;
        }

        auto object = address.as<::REManagedObject*>();

        if (object->info == nullptr || IsBadReadPtr(object->info, sizeof(void*))) {
            return false;
        }

        auto classInfo = object->info->classInfo;

        if (classInfo == nullptr || IsBadReadPtr(classInfo, sizeof(void*))) {
            return false;
        }

        return classInfo->parentInfo == object->info && classInfo->type != nullptr && classInfo->type->name != nullptr;
    }

    static REType* getType(::REManagedObject* object) {
        if (object == nullptr) {
            return nullptr;
        }

        auto info = object->info;

        if (info == nullptr) {
            return nullptr;
        }

        auto classInfo = info->classInfo;

        if (classInfo == nullptr) {
            return nullptr;
        }

        return classInfo->type;
    }

    static bool isA(::REManagedObject* object, std::string_view name) {
        if (object == nullptr) {
            return false;
        }

        for (auto t = REManagedObject::getType(object); t != nullptr && t->name != nullptr; t = t->super) {
            if (name == t->name) {
                return true;
            }
        }

        return false;
    }
}