#pragma once

#include "ReClass.hpp"

namespace utility::re_component {
    static auto get_game_object(::REComponent* comp) {
        //return utility::re_managed_object::get_field<::REGameObject*>(comp, "GameObject");
        return comp->ownerGameObject;
    }

    static auto get_valid(::REComponent* comp) {
        return utility::re_managed_object::get_field<bool>(comp, "Valid");
    }

    static auto get_chain(::REComponent* comp) {
        return utility::re_managed_object::get_field<::REComponent*>(comp, "Chain");
    }

    static auto get_delta_time(::REComponent* comp) {
        return utility::re_managed_object::get_field<float>(comp, "DeltaTime");
    }

    template<typename T = ::REComponent>
    static T* find(::REComponent* comp, std::string_view name) {
        for (auto child = comp->childComponent; child != nullptr && child != comp; child = child->childComponent) {
            if (utility::re_managed_object::is_a(child, name)) {
                return (T*)child;
            }
        }

        return nullptr;
    }

    template<typename T = ::REComponent>
    static T* find(::REComponent* comp, REType* t) {
        for (auto child = comp->childComponent; child != nullptr && child != comp; child = child->childComponent) {
            if (utility::re_managed_object::is_a(child, t)) {
                return (T*)child;
            }
        }

        return nullptr;
    }

    template<typename T = ::REComponent>
    static T** find_replaceable(::REComponent* comp, REType* t) {
        for (auto* child = &comp->childComponent; *child != nullptr && *child != comp; child = &(*child)->childComponent) {
            if (utility::re_managed_object::is_a(*child, t)) {
                return (T**)child;
            }
        }

        return nullptr;
    }

    // Find a component using the getComponent method
    /*template <typename T = ::REComponent>
    static T *find_using_method(::REComponent *comp, std::string_view name) {
        struct ClassInfoContainer {
            char padding[0x10];
            REClassInfo *info;
        } arg{};

        auto t = reframework::get_types()->get(name);

        if (t == nullptr || t->classInfo == nullptr) {
            return nullptr;
        }

        arg.info = t->classInfo;

        auto ret = utility::re_managed_object::call_method(comp->ownerGameObject, "getComponent", &arg);

        return (T *)ret->params.out_data;
    }*/
}