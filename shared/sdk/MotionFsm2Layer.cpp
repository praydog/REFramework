#include "RETypeDB.hpp"
#include "REType.hpp"
#include "REContext.hpp"
#include "Memory.hpp"
#include "RENativeArray.hpp"

#include "MotionFsm2Layer.hpp"

namespace sdk {
namespace behaviortree {
std::vector<TreeNode*> TreeNode::get_children() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<TreeNode*> out{};

    for (auto i = 0; i < tree_data->children.count; ++i) {
        const auto child_index = tree_data->children.data[i];

        auto node = tree_owner->get_node(child_index);

        if (node != nullptr) {
            out.push_back(node);
        }
    }

    return out;
}

std::vector<::REManagedObject*> TreeNode::get_unloaded_actions() const {
#if TDB_VER >= 69
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<::REManagedObject*> out{};

    for (auto i = 0; i < tree_data->actions.count; ++i) {
        const auto action_index = tree_data->actions.data[i];
        const auto action = tree_owner->get_unloaded_action(action_index);

        out.push_back(action);
    }

    return out;
#else
    return {};
#endif
}

std::vector<::REManagedObject*> TreeNode::get_actions() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<::REManagedObject*> out{};

    for (auto i = 0; i < tree_data->actions.count; ++i) {
        const auto action_index = tree_data->actions.data[i];
        const auto action = tree_owner->get_action(action_index);

        // Push it back even if it's null, because the action may not be loaded
        // and we may want to compare indices, or just want to know what the action is before it's loaded.
        out.push_back(action);
    }

    return out;
}

std::vector<::REManagedObject*> TreeNode::get_transition_conditions() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<::REManagedObject*> out{};

    for (auto i = 0; i < tree_data->transition_conditions.count; ++i) {
        const auto transition_index = tree_data->transition_conditions.data[i];
        const auto transition = tree_owner->get_condition(transition_index);

        out.push_back(transition);
    }

    return out;
}

std::vector<::REManagedObject*> TreeNode::get_transition_events() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();

    if (tree_owner == nullptr) {
        return {};
    }

    std::vector<::REManagedObject*> out{};

    for (auto i = 0; i < tree_data->start_transitions.count; ++i) {
        const auto transition_index = tree_data->start_transitions.data[i];
        const auto transition = tree_owner->get_transition(transition_index);

        out.push_back(transition);
    }

    return out;
}

std::vector<::REManagedObject*> TreeNode::get_conditions() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();

    if (tree_owner == nullptr) {
        return {};
    }

    std::vector<::REManagedObject*> out{};

    for (auto i = 0; i < tree_data->conditions.count; ++i) {
        const auto condition_index = tree_data->conditions.data[i];
        const auto condition = tree_owner->get_condition(condition_index);

         out.push_back(condition);
    }

    return out;
}

std::vector<TreeNode*> TreeNode::get_states() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<TreeNode*> out{};

    for (auto i = 0; i < tree_data->states.count; ++i) {
        const auto state_index = tree_data->states.data[i];

        auto node = tree_owner->get_node(state_index);

        out.push_back(node);
    }

    return out;
}

std::vector<TreeNode*> TreeNode::get_start_states() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<TreeNode*> out{};

    for (auto i = 0; i < tree_data->start_states.count; ++i) {
        const auto state_index = tree_data->start_states.data[i];

        auto node = tree_owner->get_node(state_index);

        out.push_back(node);
    }

    return out;
}

void TreeNode::append_action(uint32_t action_index) {
    auto tree_data = get_data();

    if (tree_data == nullptr) {
        return;
    }

    auto& arr = *(sdk::NativeArrayNoCapacity<uint32_t>*)&tree_data->actions;
    arr.push_back(action_index);


    /*auto new_array = (uint32_t*)sdk::memory::allocate(sizeof(uint32_t) * (tree_data->actions.count + 1));

    if (tree_data->actions.data != nullptr) {
        for (int32_t i = 0; i < tree_data->actions.count; ++i) {
            new_array[i] = tree_data->actions.data[i];
        }

        sdk::memory::deallocate(tree_data->actions.data);
    }

    new_array[tree_data->actions.count++] = action_index;
    tree_data->actions.data = new_array;*/
}

void TreeNode::replace_action(uint32_t index, uint32_t action_index) {
    auto tree_data = get_data();

    if (tree_data == nullptr) {
        return;
    }

    if (index >= tree_data->actions.count) {
        return;
    }

    tree_data->actions.data[index] = action_index;
}

void TreeNode::remove_action(uint32_t index) {
    auto tree_data = get_data();

    if (tree_data == nullptr) {
        return;
    }

    auto& arr = *(sdk::NativeArrayNoCapacity<uint32_t>*)&tree_data->actions;
    arr.erase(index);

    /*if (index >= tree_data->actions.count || tree_data->actions.data == nullptr) {
        return;
    }

    if (index == 0 && tree_data->actions.count == 1) {
        // easy.
        tree_data->actions.count = 0;
        return;
    }

    auto new_array = (uint32_t*)sdk::memory::allocate(sizeof(uint32_t) * (tree_data->actions.count - 1));

    for (int32_t i = 0; i < tree_data->actions.count; ++i) {
        if (i == index) {
            continue;
        }

        if (i > index) {
            new_array[i - 1] = tree_data->actions.data[i];
        } else {
            new_array[i] = tree_data->actions.data[i];
        }
    }

    sdk::memory::deallocate(tree_data->actions.data);
    --tree_data->actions.count;
    tree_data->actions.data = new_array;*/
}

void BehaviorTree::set_current_node(sdk::behaviortree::TreeNode* node, uint32_t tree_idx, void* set_node_info) {
    static auto method = sdk::find_method_definition("via.behaviortree.BehaviorTree", "setCurrentNode(System.UInt64, System.UInt32, via.behaviortree.SetNodeInfo)");

    if (method == nullptr) {
        return;
    }

    method->call(sdk::get_thread_context(), this, node->get_id(), tree_idx, set_node_info);
}

bool is_delayed() {
    #if TDB_VER >= 69
    static auto bhvt_manager = sdk::get_native_singleton("via.behaviortree.BehaviorTreeManager");
    static auto bhvt_manager_t = sdk::find_type_definition("via.behaviortree.BehaviorTreeManager");
    static auto bhvt_manager_retype = bhvt_manager_t->get_type();
    static auto is_delay_setup_objects_prop = utility::re_type::get_field_desc(bhvt_manager_retype, "DelaySetupObjects");

    const auto is_delay_setup_objects = utility::re_managed_object::get_field<bool>((::REManagedObject*)bhvt_manager, is_delay_setup_objects_prop);
#else
    const auto is_delay_setup_objects = false;
#endif

    return is_delay_setup_objects;
}

::REManagedObject* TreeObject::get_action(uint32_t index) const {
    const auto is_delay_setup_objects = is_delayed();

    if (this->data == nullptr) {
        return nullptr;
    }

    if (_bittest((const long*)&index, 30)) {
        const auto new_idx = index & 0xFFFFFFF;
        if (new_idx >= this->data->static_actions.count || this->data->static_actions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->data->static_actions.objects[new_idx];
    } else {
#if TDB_VER >= 69
        if (is_delay_setup_objects) {
            if (index >= this->delayed_actions.count || this->delayed_actions.objects == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)this->delayed_actions.objects[index];
        } else {
            if (index >= this->actions.count || this->actions.objects == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)this->actions.objects[index];
        }
#else
        if (index >= this->actions.count || this->actions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->actions.objects[index];
#endif
    }

    return nullptr;
}

::REManagedObject* TreeObject::get_unloaded_action(uint32_t index) const {
#if TDB_VER >= 69
    if (this->data == nullptr) {
        return nullptr;
    }

    if (_bittest((const long*)&index, 30)) {
        const auto new_idx = index & 0xFFFFFFF;
        if (new_idx >= this->data->static_actions.count || this->data->static_actions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->data->static_actions.objects[new_idx];
    } else {
        if (index >= this->data->actions.count || this->data->actions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->data->actions.objects[index];
    }
#endif

    return nullptr;
}

::REManagedObject* TreeObject::get_condition(int32_t index) const {
    const auto is_delay_setup_objects = is_delayed();

    if (this->data == nullptr || index == -1) {
        return nullptr;
    }

    if (_bittest((const long*)&index, 30)) {
        const auto new_idx = index & 0xFFFFFFF;
        if (new_idx >= this->data->static_conditions.count || this->data->static_conditions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->data->static_conditions.objects[new_idx];
    } else {
#if TDB_VER >= 69
        if (is_delay_setup_objects) {
            if (index >= this->delayed_conditions.count || this->delayed_conditions.objects == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)this->delayed_conditions.objects[index];
        } else {
            if (index >= this->conditions.count || this->conditions.objects == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)this->conditions.objects[index];
        }
#else
        if (index >= this->conditions.count || this->conditions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->conditions.objects[index];
#endif
    }

    return nullptr;
}

::REManagedObject* TreeObject::get_transition(int32_t index) const {
    const auto is_delay_setup_objects = is_delayed();

    if (this->data == nullptr) {
        return nullptr;
    }

    if (_bittest((const long*)&index, 30)) {
        const auto new_idx = index & 0xFFFFFFF;
        if (new_idx >= this->data->static_transitions.count || this->data->static_transitions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->data->static_transitions.objects[new_idx];
    } else {
#if TDB_VER >= 69
        if (is_delay_setup_objects) {
            if (index >= this->delayed_transitions.count || this->delayed_transitions.objects == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)this->delayed_transitions.objects[index];
        } else {
            if (index >= this->transitions.count || this->transitions.objects == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)this->transitions.objects[index];
        }
#else
        if (index >= this->transitions.count || this->transitions.objects == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)this->transitions.objects[index];
#endif 
    }

    return nullptr;
}

uint32_t TreeObject::get_action_count() const {
#if TDB_VER >= 69
    const auto is_delay_setup_objects = is_delayed();

    return is_delay_setup_objects ? this->delayed_actions.count : this->actions.count;
#else
    return this->actions.count;
#endif
}

uint32_t TreeObject::get_condition_count() const {
#if TDB_VER >= 69
    const auto is_delay_setup_objects = is_delayed();

    return is_delay_setup_objects ? this->delayed_conditions.count : this->conditions.count;
#else
    return this->conditions.count;
#endif
}

uint32_t TreeObject::get_transition_count() const {
#if TDB_VER >= 69
    const auto is_delay_setup_objects = is_delayed();

    return is_delay_setup_objects ? this->delayed_transitions.count : this->transitions.count;
#else
    return this->transitions.count;
#endif
}
}
}