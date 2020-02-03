#pragma once

#include "ReClass.hpp"

namespace utility::REComponent {
    static auto getGameObject(::REComponent* comp) {
        return utility::REManagedObject::getField<::REGameObject*>(comp, "GameObject");
    }

    static auto getValid(::REComponent* comp) {
        return utility::REManagedObject::getField<bool>(comp, "Valid");
    }

    static auto getChain(::REComponent* comp) {
        return utility::REManagedObject::getField<::REComponent*>(comp, "Chain");
    }

    static auto getDeltaTime(::REComponent* comp) {
        return utility::REManagedObject::getField<float>(comp, "DeltaTime");
    }

    template<typename T = ::REComponent>
    static T* find(::REComponent* comp, std::string_view name) {
        for (auto child = comp->childComponent; child != nullptr && child != comp; child = child->childComponent) {
            if (REManagedObject::isA(child, name)) {
                return (T*)child;
            }
        }

        return nullptr;
    }

    // Find a component using the getComponent method
    template <typename T = ::REComponent>
    static T *findUsingMethod(::REComponent *comp, std::string_view name) {
        struct ClassInfoContainer {
            char padding[0x10];
            REClassInfo *info;
        } arg{};

        auto t = g_framework->getTypes()->get(name);

        if (t == nullptr || t->classInfo == nullptr) {
            return nullptr;
        }

        arg.info = t->classInfo;

        auto ret = utility::REManagedObject::callMethod(comp->ownerGameObject, "getComponent", &arg);

        return (T *)ret->params.out_data;
    }
}