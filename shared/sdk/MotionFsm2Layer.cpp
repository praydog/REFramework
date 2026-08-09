#include <spdlog/spdlog.h>

#include <utility/Relocate.hpp>

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
    auto& children = tree_data->get_children();

    for (uint64_t i = 0; i < children.size(); ++i) {
        const auto child_index = children[i];
        auto node = tree_owner->get_node(child_index);

        if (node != nullptr) {
            out.push_back(node);
        }
    }

    return out;
}

std::vector<::REManagedObject*> TreeNode::get_unloaded_actions() const {
    if (sdk::GameIdentity::get().tdb_ver() >= 69) {
        const auto tree_data = get_data();

        if (tree_data == nullptr) {
            return {};
        }

        const auto tree_owner = this->get_owner();
    
        if (tree_owner == nullptr) {
            return {};
        }
    
        std::vector<::REManagedObject*> out{};
        auto& actions = tree_data->get_actions();

        for (uint64_t i = 0; i < actions.size(); ++i) {
            const auto action_index = actions[i];
            const auto action = tree_owner->get_unloaded_action(action_index);

            out.push_back(action);
        }

        return out;
    } else {
        return {};
    }
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
    auto& actions = tree_data->get_actions();

    for (uint64_t i = 0; i < actions.size(); ++i) {
        const auto action_index = actions[i];
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
    auto& tc = tree_data->get_transition_conditions();

    for (uint64_t i = 0; i < tc.size(); ++i) {
        const auto transition_index = tc[i];
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
    auto& st = tree_data->get_start_transitions();

    for (uint64_t i = 0; i < st.size(); ++i) {
        const auto transition_index = st[i];
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
    auto& conds = tree_data->get_conditions();

    for (uint64_t i = 0; i < conds.size(); ++i) {
        const auto condition_index = conds[i];
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
    auto& states = tree_data->get_states();

    for (uint64_t i = 0; i < states.size(); ++i) {
        const auto state_index = states[i];
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
    auto& ss = tree_data->get_start_states();

    for (uint64_t i = 0; i < ss.size(); ++i) {
        const auto state_index = ss[i];
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

    auto& arr = tree_data->get_actions();
    arr.push_back(action_index);
}

void TreeNode::replace_action(uint32_t index, uint32_t action_index) {
    auto tree_data = get_data();

    if (tree_data == nullptr) {
        return;
    }

    auto& arr = tree_data->get_actions();
    if (index >= arr.size()) {
        return;
    }

    arr[index] = action_index;
}

void TreeNode::remove_action(uint32_t index) {
    auto tree_data = get_data();

    if (tree_data == nullptr) {
        return;
    }

    auto& arr = tree_data->get_actions();
    arr.erase(index);
}

void TreeNode::relocate(uintptr_t old_start, uintptr_t old_end, uintptr_t new_start) {
    auto selector = (::REManagedObject*)get_selector();

    if (selector != nullptr && REManagedObject::is_managed_object(selector)) {
        const auto td = selector->get_type_definition();

        if (td != nullptr) {
            utility::relocate_pointers((uint8_t*)selector, old_start, old_end, new_start, 1, sizeof(void*), td->get_size());
        }
    }

    utility::relocate_pointers((uint8_t*)this, old_start, old_end, new_start, 0, sizeof(void*), tree_node_stride());
}

void BehaviorTree::set_current_node(sdk::behaviortree::TreeNode* node, uint32_t tree_idx, void* set_node_info) {
    static auto method = sdk::find_method_definition("via.behaviortree.BehaviorTree", "setCurrentNode(System.UInt64, System.UInt32, via.behaviortree.SetNodeInfo)");

    if (method == nullptr) {
        return;
    }

    method->call(sdk::get_thread_context(), this, node->get_id(), tree_idx, set_node_info);
}

bool TreeObject::is_delayed() const {
    if (sdk::GameIdentity::get().tdb_ver() >= 69) {
        static auto bhvt_manager = sdk::get_native_singleton("via.behaviortree.BehaviorTreeManager");
        static auto bhvt_manager_t = sdk::find_type_definition("via.behaviortree.BehaviorTreeManager");
        static auto bhvt_manager_retype = bhvt_manager_t->get_type();
        static auto is_delay_setup_objects_prop = utility::re_type::get_field_desc(bhvt_manager_retype, "DelaySetupObjects");

        const auto is_delay_setup_objects = ((::REManagedObject*)bhvt_manager)->get_reflection_property<bool>(is_delay_setup_objects_prop);

        if (!is_delay_setup_objects) {
            auto& actions = get_action_array();
            auto& conditions = get_condition_array();
            auto& transitions = get_transition_array();
            auto& delayed_actions = get_delayed_actions();
            auto& delayed_conditions = get_delayed_conditions();
            auto& delayed_transitions = get_delayed_transitions();

            // For some reason this can happen
            if (actions.size() == 0 && conditions.size() == 0 && transitions.size() == 0) {
                if (delayed_actions.size() > 0 || delayed_conditions.size() > 0 || delayed_transitions.size() > 0) {
                    return true;
                }
            }
        }

        return is_delay_setup_objects;
    } else {
        return false;
    }
}

void TreeObject::relocate(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNode>& new_nodes) {
    const auto new_start = (uintptr_t)new_nodes.elements;
    const auto node_count = new_nodes.num;
    const auto stride = tree_node_stride();

    for (uint32_t i = 0; i < node_count; i++) {
        auto* node = (TreeNode*)((uint8_t*)new_nodes.elements + i * stride);
        node->relocate(old_start, old_end, new_start);
    }

    // TreeObject is 0xD8 in both layouts
    utility::relocate_pointers((uint8_t*)this, old_start, old_end, new_start, 1, sizeof(void*), 0xD8);

    set_root_node((TreeNode*)new_nodes.elements);
}

void TreeObject::relocate_datas(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNodeData>& new_nodes) {
    const auto new_start = (uintptr_t)new_nodes.elements;

    // Fix the pointers that point to the data inside the actual nodes (not the datas themselves).
    const auto node_count = get_node_count();
    auto* nodes_base = get_nodes_ptr();
    const auto total_nodes_bytes = node_count * tree_node_stride();

    utility::relocate_pointers((uint8_t*)nodes_base, old_start, old_end, new_start, 1, sizeof(void*), total_nodes_bytes);
    utility::relocate_pointers((uint8_t*)this, old_start, old_end, new_start, 1, sizeof(void*), 0xD8);
}

::REManagedObject* TreeObject::get_uservariable_hub() const {
    static uint32_t uservar_hub_offset = [this]() -> uint32_t {
        spdlog::info("Searching for uservar hub offset...");

        // TreeObject is 0xD8 in both layouts
        for (uint32_t i = 0; i < 0xD8; i += sizeof(void*)) {
            const auto obj = *(::REManagedObject**)((uintptr_t)this + i);

            if (obj == nullptr || !REManagedObject::is_managed_object(obj)) {
                continue;
            }

            if (obj->is_a("via.userdata.UserVariablesHub")) {
                spdlog::info("Found uservar hub at offset {:x}", i);
                return i;
            }
        }

        spdlog::error("Failed to find UserVariablesHub in TreeObject");
        return 0;
    }();

    return *(::REManagedObject**)((uintptr_t)this + uservar_hub_offset);
}

void CoreHandle::relocate(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNode>& new_nodes) {
    const auto new_start = (uintptr_t)new_nodes.elements;

    this->get_tree_object()->relocate(old_start, old_end, new_nodes);

    utility::relocate_pointers((uint8_t*)this, old_start, old_end, new_start, 1, sizeof(void*), sizeof(*this));
}

void CoreHandle::relocate_datas(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNodeData>& new_nodes) {
    const auto new_start = (uintptr_t)new_nodes.elements;

    this->get_tree_object()->relocate_datas(old_start, old_end, new_nodes);

    utility::relocate_pointers((uint8_t*)this, old_start, old_end, new_start, 1, sizeof(void*), sizeof(*this));
}

::REManagedObject* TreeObject::get_action(uint32_t index) const {
    const auto is_delay_setup_objects = is_delayed();
    const auto data = get_data();

    if (data == nullptr) {
        return nullptr;
    }

    if (_bittest((const long*)&index, 30)) {
        const auto new_idx = index & 0xFFFFFFF;
        auto& sa = data->get_static_actions();
        if (new_idx >= sa.size() || sa.elements == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)sa[new_idx];
    } else {
        if (sdk::GameIdentity::get().tdb_ver() >= 69) {
            if (is_delay_setup_objects) {
                auto& da = get_delayed_actions();
                if (index >= da.size() || da.elements == nullptr) {
                    return nullptr;
                }

                return (::REManagedObject*)da[index];
            } else {
                auto& a = get_action_array();
                if (index >= a.size() || a.elements == nullptr) {
                    return nullptr;
                }

                return (::REManagedObject*)a[index];
            }
        } else {
            auto& a = get_action_array();
            if (index >= a.size() || a.elements == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)a[index];
        }
    }

    return nullptr;
}

::REManagedObject* TreeObject::get_unloaded_action(uint32_t index) const {
    if (sdk::GameIdentity::get().tdb_ver() >= 69) {
        const auto data = get_data();
        if (data == nullptr) {
            return nullptr;
        }

        if (_bittest((const long*)&index, 30)) {
            const auto new_idx = index & 0xFFFFFFF;
            auto& sa = data->get_static_actions();
            if (new_idx >= sa.size() || sa.elements == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)sa[new_idx];
        } else {
            auto& a = data->get_actions();
            if (index >= a.size() || a.elements == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)a[index];
        }
    }

    return nullptr;
}

::REManagedObject* TreeObject::get_condition(int32_t index) const {
    const auto is_delay_setup_objects = is_delayed();
    const auto data = get_data();

    if (data == nullptr || index == -1) {
        return nullptr;
    }

    if (_bittest((const long*)&index, 30)) {
        const auto new_idx = index & 0xFFFFFFF;
        auto& sc = data->get_static_conditions();
        if (new_idx >= sc.size() || sc.elements == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)sc[new_idx];
    } else {
        if (sdk::GameIdentity::get().tdb_ver() >= 69) {
            if (is_delay_setup_objects) {
                auto& dc = get_delayed_conditions();
                if (index >= (int32_t)dc.size() || dc.elements == nullptr) {
                    return nullptr;
                }

                return (::REManagedObject*)dc[index];
            } else {
                auto& c = get_condition_array();
                if (index >= (int32_t)c.size() || c.elements == nullptr) {
                    return nullptr;
                }

                return (::REManagedObject*)c[index];
            }
        } else {
            auto& c = get_condition_array();
            if (index >= (int32_t)c.size() || c.elements == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)c[index];
        }
    }

    return nullptr;
}

::REManagedObject* TreeObject::get_transition(int32_t index) const {
    const auto is_delay_setup_objects = is_delayed();
    const auto data = get_data();

    if (data == nullptr) {
        return nullptr;
    }

    if (_bittest((const long*)&index, 30)) {
        const auto new_idx = index & 0xFFFFFFF;
        auto& st = data->get_static_transitions();
        if (new_idx >= st.size() || st.elements == nullptr) {
            return nullptr;
        }

        return (::REManagedObject*)st[new_idx];
    } else {
        if (sdk::GameIdentity::get().tdb_ver() >= 69) {
            if (is_delay_setup_objects) {
                auto& dt = get_delayed_transitions();
                if (index >= (int32_t)dt.size() || dt.elements == nullptr) {
                    return nullptr;
                }

                return (::REManagedObject*)dt[index];
            } else {
                auto& t = get_transition_array();
                if (index >= (int32_t)t.size() || t.elements == nullptr) {
                    return nullptr;
                }

                return (::REManagedObject*)t[index];
            }
        } else {
            auto& t = get_transition_array();
            if (index >= (int32_t)t.size() || t.elements == nullptr) {
                return nullptr;
            }

            return (::REManagedObject*)t[index];
        }
    }

    return nullptr;
}

uint32_t TreeObject::get_action_count() const {
    const auto is_delay_setup_objects = is_delayed();
    return is_delay_setup_objects ? get_delayed_actions().size() : get_action_array().size();
}

uint32_t TreeObject::get_condition_count() const {
    const auto is_delay_setup_objects = is_delayed();
    return is_delay_setup_objects ? get_delayed_conditions().size() : get_condition_array().size();
}

uint32_t TreeObject::get_transition_count() const {
    const auto is_delay_setup_objects = is_delayed();
    return is_delay_setup_objects ? get_delayed_transitions().size() : get_transition_array().size();
}
}
}
