#pragma once

#include <cstdint>
#include <vector>

#include "REString.hpp"
#include "RENativeArray.hpp"

#if TDB_VER >= 69
#include "regenny/mhrise_tdb71/via/behaviortree/BehaviorTreeCoreHandleArray.hpp"
#include "regenny/mhrise_tdb71/via/motion/MotionFsm2Layer.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeNodeData.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeNode.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeObjectData.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeObject.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/BehaviorTree.hpp"
/*#elif TDB_VER >= 69
#include "regenny/re2_tdb70/via/behaviortree/BehaviorTreeCoreHandleArray.hpp"
#include "regenny/re2_tdb70/via/motion/MotionFsm2Layer.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeNodeData.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeNode.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeObjectData.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeObject.hpp"
#include "regenny/re2_tdb70/via/behaviortree/BehaviorTree.hpp"*/
#else
#include "regenny/re3/via/behaviortree/TreeObjectData.hpp"
#include "regenny/re3/via/behaviortree/BehaviorTreeCoreHandleArray.hpp"
#include "regenny/re3/via/motion/MotionFsm2Layer.hpp"
#include "regenny/re3/via/behaviortree/TreeNodeData.hpp"
#include "regenny/re3/via/behaviortree/TreeNode.hpp"
#include "regenny/re3/via/behaviortree/TreeObject.hpp"
#include "regenny/re3/via/behaviortree/BehaviorTree.hpp"
#endif

namespace sdk {
class MotionFsm2Layer;

namespace behaviortree {
bool is_delayed();

class TreeNodeData;
class TreeNode;
class TreeObject;

class TreeNodeData : public regenny::via::behaviortree::TreeNodeData {
public:
    sdk::NativeArrayNoCapacity<uint32_t>& get_children() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->children;
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_actions() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->actions;
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_states() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->states;
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_states_2() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->states_2;
    }

    sdk::NativeArrayNoCapacity<int32_t>& get_start_transitions() {
        return *(sdk::NativeArrayNoCapacity<int32_t>*)&this->start_transitions;
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_start_states() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->start_states;
    }

    sdk::NativeArrayNoCapacity<int32_t>& get_conditions() {
        return *(sdk::NativeArrayNoCapacity<int32_t>*)&this->conditions;
    }

    sdk::NativeArrayNoCapacity<int32_t>& get_transition_conditions() {
        return *(sdk::NativeArrayNoCapacity<int32_t>*)&this->transition_conditions;
    }

    sdk::NativeArrayNoCapacity<sdk::NativeArrayNoCapacity<uint32_t>>& get_transition_events() {
        return *(sdk::NativeArrayNoCapacity<sdk::NativeArrayNoCapacity<uint32_t>>*)&this->transition_events;
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_transition_ids() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->transition_ids;
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_transition_attributes() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->transition_attributes;
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_tags() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)&this->tags;
    }

    std::wstring_view get_name() const {
        const auto& re_string = *(::REString*)&this->name;
        return utility::re_string::get_view(re_string);
    }

    void set_name(const std::wstring& name) {
        auto& re_string = *(::REString*)&this->name;
        utility::re_string::set_string(re_string, name);
    }
};

class TreeNode : public regenny::via::behaviortree::TreeNode {
public:
    std::vector<TreeNode*> get_children() const;
    std::vector<::REManagedObject*> get_unloaded_actions() const;
    std::vector<::REManagedObject*> get_actions() const;
    std::vector<::REManagedObject*> get_transition_conditions() const;
    std::vector<::REManagedObject*> get_transition_events() const;
    std::vector<::REManagedObject*> get_conditions() const;
    std::vector<TreeNode*> get_states() const;
    std::vector<TreeNode*> get_start_states() const;
    
    void append_action(uint32_t action_index);
    void replace_action(uint32_t index, uint32_t action_index);
    void remove_action(uint32_t index);

    // Getters just in-case we decide to dynamically generate the structure layout
    // instead of using manual offsets.
    uint64_t get_id() const {
        return this->id;
    }

    sdk::behaviortree::TreeNodeData* get_data() const {
        return (sdk::behaviortree::TreeNodeData*)this->data;
    }

    sdk::behaviortree::TreeObject* get_owner() const {
        return (sdk::behaviortree::TreeObject*)this->owner;
    }

    sdk::behaviortree::TreeNode* get_parent() const {
        return (sdk::behaviortree::TreeNode*)this->node_parent;
    }

    uint16_t get_attr() const {
        return this->attr;
    }

    regenny::via::behaviortree::SelectorFSM* get_selector() const {
        return this->selector;
    }

    regenny::via::behaviortree::Condition* get_selector_condition() const {
        return this->selector_condition;
    }

    int32_t get_selector_condition_index() const {
        return this->selector_condition_index;
    }

    int32_t get_priority() const {
        return this->priority;
    }

    sdk::behaviortree::TreeNode* get_node_parent() const {
        return (sdk::behaviortree::TreeNode*)this->node_parent;
    }

    regenny::via::behaviortree::Condition* get_parent_condition() const {
#if TDB_VER >= 69
        return this->parent_condition;
#else
        return nullptr;
#endif
    }

    regenny::via::behaviortree::NodeStatus get_status1() const {
        return this->status1;
    }

    regenny::via::behaviortree::NodeStatus get_status2() const {
        return this->status2;
    }

    std::wstring_view get_name() const {
        const auto& str = *(::REString*)&this->name;

        return utility::re_string::get_view(str);
    }

    void set_name(const std::wstring& name) {
        auto& str = *(::REString*)&this->name;
        utility::re_string::set_string(str, name);
    }

    std::wstring get_full_name() const {
        auto out = std::wstring{this->get_name()};
        auto cur = this->get_parent();
        
        while (cur != nullptr && cur->get_name() != L"root" && cur != this) {
            out = std::wstring{cur->get_name()} + L"." + out;
            cur = cur->get_parent();
        }

        return out;
    }

    void relocate(uintptr_t old_start, uintptr_t old_end, uintptr_t new_start);
};

class TreeObjectData : public regenny::via::behaviortree::TreeObjectData {
public:
    sdk::NativeArrayNoCapacity<TreeNodeData>& get_nodes() {
        return *(sdk::NativeArrayNoCapacity<TreeNodeData>*)&this->nodes;
    }

    sdk::NativeArray<::REManagedObject*>& get_static_actions() { 
        return *(sdk::NativeArray<::REManagedObject*>*)&this->static_actions;
    }

    sdk::NativeArray<::REManagedObject*>& get_static_conditions() {
        return *(sdk::NativeArray<::REManagedObject*>*)&this->static_conditions;
    }

    sdk::NativeArray<::REManagedObject*>& get_static_transitions() {
        return *(sdk::NativeArray<::REManagedObject*>*)&this->static_transitions;
    }

    sdk::NativeArray<::REManagedObject*>& get_expression_tree_conditions() {
        return *(sdk::NativeArray<::REManagedObject*>*)&this->expression_tree_conditions;
    }

    sdk::NativeArrayNoCapacity<uint8_t>& get_action_methods() {
        return *(sdk::NativeArrayNoCapacity<uint8_t>*)&this->action_methods;
    }

    sdk::NativeArrayNoCapacity<uint8_t>& get_static_action_methods() {
        return *(sdk::NativeArrayNoCapacity<uint8_t>*)&this->static_action_methods;
    }
};

class TreeObject : public regenny::via::behaviortree::TreeObject {
public:
    bool is_delayed() const;
    void relocate(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNode>& new_nodes);
    void relocate_datas(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNodeData>& new_nodes);

    ::REManagedObject* get_uservariable_hub() const;

    sdk::behaviortree::TreeObjectData* get_data() const {
        return (sdk::behaviortree::TreeObjectData*)this->data;
    }

    sdk::behaviortree::TreeNode* begin() const {
        if (this->nodes.count <= 0 || this->nodes.nodes == nullptr) {
            return nullptr;
        }

        return (sdk::behaviortree::TreeNode*)&this->nodes.nodes[0];
    }

    sdk::behaviortree::TreeNode* end() const {
        if (this->nodes.count <= 0 || this->nodes.nodes == nullptr) {
            return nullptr;
        }

        return (sdk::behaviortree::TreeNode*)&this->nodes.nodes[this->nodes.count];
    }

    sdk::NativeArrayNoCapacity<TreeNode>& get_node_array() {
        return *(sdk::NativeArrayNoCapacity<TreeNode>*)&this->nodes;
    }

    sdk::NativeArray<::REManagedObject*>& get_condition_array() {
        return *(sdk::NativeArray<::REManagedObject*>*)&this->conditions;
    }

    sdk::NativeArray<::REManagedObject*>& get_action_array() {
        return *(sdk::NativeArray<::REManagedObject*>*)&this->actions;
    }

    sdk::NativeArray<::REManagedObject*>& get_transition_array() {
        return *(sdk::NativeArray<::REManagedObject*>*)&this->transitions;
    }

    sdk::NativeArray<::REManagedObject*>& get_selector_array() {
        return *(sdk::NativeArray<::REManagedObject*>*)&this->selectors;
    }

    bool empty() const {
        if (this->nodes.count <= 0 || this->nodes.nodes == nullptr) {
            return true;
        }

        return false;
    }

    sdk::behaviortree::TreeNode* get_node(uint32_t index) const {
        if (this->nodes.count <= index || this->nodes.nodes == nullptr) {
            return nullptr;
        }

        return (sdk::behaviortree::TreeNode*)&this->nodes.nodes[index];
    }

    uint32_t get_node_count() const {
        return this->nodes.count;
    }

    std::vector<sdk::behaviortree::TreeNode*> get_nodes() const {
        std::vector<sdk::behaviortree::TreeNode*> out{};

        for (uint32_t i = 0; i < this->nodes.count; i++) {
            out.push_back((sdk::behaviortree::TreeNode*)&this->nodes.nodes[i]);
        }
        
        return out;
    }

    sdk::behaviortree::TreeNode* get_node_by_name(std::wstring_view name) const {
        if (this->empty()) {
            return nullptr;
        }

        for (auto& node : *this) {
            if (node.get_name() == name) {
                return &node;
            }
        }

        return nullptr;
    }

    sdk::behaviortree::TreeNode* get_node_by_name(std::string_view name) const {
        return get_node_by_name(utility::widen(name));
    }

    sdk::behaviortree::TreeNode* get_node_by_id(uint64_t id) const {
        if (this->empty()) {
            return nullptr;
        }

        for (auto& node : *this) {
            if (node.id == id) {
                return &node;
            }
        }

        return nullptr;
    }

    ::REManagedObject* get_action(uint32_t index) const;
    ::REManagedObject* get_unloaded_action(uint32_t index) const;
    ::REManagedObject* get_condition(int32_t index) const;
    ::REManagedObject* get_transition(int32_t index) const;

    uint32_t get_action_count() const;
    uint32_t get_condition_count() const;
    uint32_t get_transition_count() const;

    uint32_t get_unloaded_action_count() const {
#if TDB_VER >= 69
        const auto data = get_data();

        if (data == nullptr) {
            return 0;
        }

        return data->actions.count;
#else
        return 0;
#endif
    }

    uint32_t get_static_action_count() const {
        const auto data = get_data();

        if (data == nullptr) {
            return 0;
        }

        return data->static_actions.count;
    }

    uint32_t get_static_condition_count() const {
        const auto data = get_data();

        if (data == nullptr) {
            return 0;
        }

        return data->static_conditions.count;
    }

    uint32_t get_static_transition_count() const {
        const auto data = get_data();

        if (data == nullptr) {
            return 0;
        }

        return data->static_transitions.count;
    }
};

class CoreHandle : public regenny::via::behaviortree::CoreHandle {
public:
    sdk::behaviortree::TreeObject* get_tree_object() const {
        return (sdk::behaviortree::TreeObject*)this->core.tree_object;
    }

    void relocate(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNode>& new_nodes);
    void relocate_datas(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNodeData>& new_nodes);
};

class BehaviorTree : public regenny::via::behaviortree::BehaviorTree {
public:
    // Called by vtable index 11 or 12?
    template<typename T = sdk::behaviortree::CoreHandle>
    T* get_tree(uint32_t index) const {
        if (this->trees.count <= index || this->trees.handles == nullptr) {
            return nullptr;
        }

        return (T*)this->trees.handles[index];
    }

    uint32_t get_tree_count() const {
        return this->trees.count;
    }

    template<typename T = sdk::behaviortree::CoreHandle>
    std::vector<T*> get_trees() const {
        std::vector<T*> out{};

        for (uint32_t i = 0; i < this->trees.count; i++) {
            out.push_back((T*)this->trees.handles[i]);
        }
        
        return out;
    }

    void set_current_node(sdk::behaviortree::TreeNode* node, uint32_t tree_idx, void* set_node_info = nullptr);
};
};

class MotionFsm2Layer : public regenny::via::motion::MotionFsm2Layer {
public:
    sdk::behaviortree::TreeObject* get_tree_object() const {
        return (sdk::behaviortree::TreeObject*)this->core.tree_object;
    }
};
}