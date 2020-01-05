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
}