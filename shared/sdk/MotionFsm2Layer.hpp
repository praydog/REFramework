#pragma once

#include <cstdint>
#include <vector>

#include "REString.hpp"
#include "RENativeArray.hpp"
#include "GameIdentity.hpp"

// TDB >= 69 (RE8, MHRise, SF6, DD2, etc.) — TreeNode is 0xD0, has parent_condition field
// TDB < 69 (DMC5, RE3) — TreeNode is 0xC8, no parent_condition. Offsets shift.
// The regenny headers describe the TDB71 layout and are used via reinterpret_cast
// inside accessor implementations for stable-offset fields.
#include "regenny/mhrise_tdb71/via/behaviortree/BehaviorTreeCoreHandleArray.hpp"
#include "regenny/mhrise_tdb71/via/motion/MotionFsm2Layer.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeNodeData.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeNode.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeObjectData.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/TreeObject.hpp"
#include "regenny/mhrise_tdb71/via/behaviortree/BehaviorTree.hpp"

namespace sdk {
class MotionFsm2Layer;

namespace behaviortree {

// Runtime dispatch for TDB version differences in behavior tree structs.
inline bool needs_legacy_bhvt() {
    return sdk::GameIdentity::get().tdb_ver() < 69;
}

// Stride sizes for array iteration — the actual in-memory size of each struct.
inline size_t tree_node_stride() { return needs_legacy_bhvt() ? 0xC8 : 0xD0; }
inline size_t tree_node_data_stride() { return needs_legacy_bhvt() ? 0x1C0 : 0x1D0; }

bool is_delayed();

class TreeNodeData;
class TreeNode;
class TreeObject;
class TreeObjectData;

// Helper to get a TreeNode* at a given index using stride arithmetic.
inline TreeNode* tree_node_at(void* base, uint32_t index) {
    return (TreeNode*)((uint8_t*)base + index * tree_node_stride());
}

inline TreeNodeData* tree_node_data_at(void* base, uint32_t index) {
    return (TreeNodeData*)((uint8_t*)base + index * tree_node_data_stride());
}

// Opaque wrapper — field access exclusively through accessors.
// Offsets differ between TDB < 69 and TDB >= 69 for: tags, name.
class TreeNodeData {
public:
    // --- Stable-offset accessors (same in both layouts) ---
    uint32_t get_id() const { return *(uint32_t*)((uintptr_t)this + 0x0); }
    uint32_t get_id_2() const { return *(uint32_t*)((uintptr_t)this + 0x4); }
    uint32_t get_attr() const { return *(uint32_t*)((uintptr_t)this + 0x8); }
    bool get_is_branch() const { return *(bool*)((uintptr_t)this + 0xC); }
    bool get_is_end() const { return *(bool*)((uintptr_t)this + 0xD); }
    bool get_has_selector() const { return *(bool*)((uintptr_t)this + 0xE); }
    uint32_t get_parent() const { return *(uint32_t*)((uintptr_t)this + 0x34); }
    uint32_t get_parent_2() const { return *(uint32_t*)((uintptr_t)this + 0x38); }


    sdk::NativeArrayNoCapacity<uint32_t>& get_children() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + 0x40);
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_actions() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + 0xC0);
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_states() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + 0x70);
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_states_2() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + 0x80);
    }

    sdk::NativeArrayNoCapacity<int32_t>& get_start_transitions() {
        return *(sdk::NativeArrayNoCapacity<int32_t>*)((uintptr_t)this + 0x130);
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_start_states() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + 0x140);
    }

    sdk::NativeArrayNoCapacity<int32_t>& get_conditions() {
        return *(sdk::NativeArrayNoCapacity<int32_t>*)((uintptr_t)this + 0x60);
    }

    sdk::NativeArrayNoCapacity<int32_t>& get_transition_conditions() {
        return *(sdk::NativeArrayNoCapacity<int32_t>*)((uintptr_t)this + 0x90);
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_transition_ids() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + 0xA0);
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_transition_attributes() {
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + 0xB0);
    }

    // --- Dispatching accessors (offset differs by TDB version) ---

    // transition_events: offset 0x160 stable, but type differs.
    // TDB71: UIntArrayArray (array of arrays). TDB<69: UIntArray (flat).
    // We only expose the TDB71 semantics; returns empty on legacy.
    sdk::NativeArrayNoCapacity<sdk::NativeArrayNoCapacity<uint32_t>>& get_transition_events() {
        // Offset is 0x160 in both, but the flat UIntArray on legacy is not type-compatible.
        // On legacy this will have the right pointer shape but wrong semantics.
        // Callers already gate on tdb_ver >= 69 for transition_events usage.
        return *(sdk::NativeArrayNoCapacity<sdk::NativeArrayNoCapacity<uint32_t>>*)((uintptr_t)this + 0x160);
    }

    sdk::NativeArrayNoCapacity<uint32_t>& get_tags() {
        const auto offset = needs_legacy_bhvt() ? 0x180 : 0x190;
        return *(sdk::NativeArrayNoCapacity<uint32_t>*)((uintptr_t)this + offset);
    }

    std::wstring_view get_name() const {
        const auto offset = needs_legacy_bhvt() ? 0x190 : 0x1A0;
        const auto& re_string = *(::REString*)((uintptr_t)this + offset);
        return utility::re_string::get_view(re_string);
    }

    void set_name(const std::wstring& name) {
        const auto offset = needs_legacy_bhvt() ? 0x190 : 0x1A0;
        auto& re_string = *(::REString*)((uintptr_t)this + offset);
        utility::re_string::set_string(re_string, name);
    }

    static size_t stride() { return tree_node_data_stride(); }

private:
    TreeNodeData() = delete;
    TreeNodeData(const TreeNodeData&) = delete;
    TreeNodeData& operator=(const TreeNodeData&) = delete;
};

// Opaque wrapper — field access exclusively through accessors.
// parent_condition field only exists on TDB >= 69.
// status1, status2, name are shifted between layouts.
class TreeNode {
public:
    // --- Stable-offset accessors ---
    uint64_t get_id() const {
        return *(uint64_t*)((uintptr_t)this + 0x0);
    }

    sdk::behaviortree::TreeNodeData* get_data() const {
        return *(sdk::behaviortree::TreeNodeData**)((uintptr_t)this + 0x8);
    }

    sdk::behaviortree::TreeObject* get_owner() const {
        return *(sdk::behaviortree::TreeObject**)((uintptr_t)this + 0x10);
    }

    uint16_t get_attr() const {
        return *(uint16_t*)((uintptr_t)this + 0x18);
    }

    regenny::via::behaviortree::SelectorFSM* get_selector() const {
        return *(regenny::via::behaviortree::SelectorFSM**)((uintptr_t)this + 0x20);
    }

    regenny::via::behaviortree::Condition* get_selector_condition() const {
        return *(regenny::via::behaviortree::Condition**)((uintptr_t)this + 0x28);
    }

    int32_t get_selector_condition_index() const {
        return *(int32_t*)((uintptr_t)this + 0x30);
    }

    int32_t get_priority() const {
        return *(int32_t*)((uintptr_t)this + 0x34);
    }

    sdk::behaviortree::TreeNode* get_parent() const {
        return *(sdk::behaviortree::TreeNode**)((uintptr_t)this + 0x38);
    }

    sdk::behaviortree::TreeNode* get_node_parent() const {
        return get_parent();
    }

    // --- Dispatching accessors ---

    // parent_condition: only exists in TDB >= 69 at offset 0x58.
    regenny::via::behaviortree::Condition* get_parent_condition() const {
        if (needs_legacy_bhvt()) {
            return nullptr;
        }
        return *(regenny::via::behaviortree::Condition**)((uintptr_t)this + 0x58);
    }

    regenny::via::behaviortree::NodeStatus get_status1() const {
        const auto offset = needs_legacy_bhvt() ? 0x5C : 0x64;
        return *(regenny::via::behaviortree::NodeStatus*)((uintptr_t)this + offset);
    }

    regenny::via::behaviortree::NodeStatus get_status2() const {
        const auto offset = needs_legacy_bhvt() ? 0x60 : 0x68;
        return *(regenny::via::behaviortree::NodeStatus*)((uintptr_t)this + offset);
    }

    std::wstring_view get_name() const {
        const auto offset = needs_legacy_bhvt() ? 0x68 : 0x70;
        const auto& str = *(::REString*)((uintptr_t)this + offset);
        return utility::re_string::get_view(str);
    }

    void set_name(const std::wstring& name) {
        const auto offset = needs_legacy_bhvt() ? 0x68 : 0x70;
        auto& str = *(::REString*)((uintptr_t)this + offset);
        utility::re_string::set_string(str, name);
    }

    // --- Derived accessors (delegate to other classes) ---
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

    static size_t stride() { return tree_node_stride(); }

private:
    TreeNode() = delete;
    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;
};

// Opaque wrapper for TreeObjectData.
// TDB71-only fields: actions@0x68, conditions@0x78, transitions@0x88.
// Shifted fields: action_methods, static_action_methods.
class TreeObjectData {
public:
    // nodes pointer at 0x8, count at 0x10
    TreeNodeData* get_nodes_ptr() const {
        return *(TreeNodeData**)((uintptr_t)this + 0x8);
    }

    uint32_t get_node_count() const {
        return *(uint32_t*)((uintptr_t)this + 0x10);
    }

    // Legacy compatibility: returns the NativeArrayNoCapacity view at offset 0x8.
    // WARNING: iteration via begin/end uses sizeof(TreeNodeData) which is wrong.
    // Use get_nodes_ptr() + tree_node_data_at() for stride-aware access.
    sdk::NativeArrayNoCapacity<TreeNodeData>& get_nodes() {
        return *(sdk::NativeArrayNoCapacity<TreeNodeData>*)((uintptr_t)this + 0x8);
    }

    // --- Stable-offset accessors ---
    sdk::NativeArray<::REManagedObject*>& get_static_actions() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x28);
    }

    sdk::NativeArray<::REManagedObject*>& get_static_conditions() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x38);
    }

    sdk::NativeArray<::REManagedObject*>& get_static_transitions() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x48);
    }

    sdk::NativeArray<::REManagedObject*>& get_expression_tree_conditions() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x58);
    }

    // --- Dispatching accessors ---

    // actions/conditions/transitions at 0x68/0x78/0x88 only exist on TDB >= 69.
    // Returns a reference to the array if present, or a static empty one on legacy.
    sdk::NativeArray<::REManagedObject*>& get_actions() const {
        if (needs_legacy_bhvt()) {
            static sdk::NativeArray<::REManagedObject*> empty{};
            return empty;
        }
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x68);
    }

    sdk::NativeArrayNoCapacity<uint8_t>& get_action_methods() {
        const auto offset = needs_legacy_bhvt() ? 0xC8 : 0xF8;
        return *(sdk::NativeArrayNoCapacity<uint8_t>*)((uintptr_t)this + offset);
    }

    sdk::NativeArrayNoCapacity<uint8_t>& get_static_action_methods() {
        const auto offset = needs_legacy_bhvt() ? 0xD8 : 0x108;
        return *(sdk::NativeArrayNoCapacity<uint8_t>*)((uintptr_t)this + offset);
    }

private:
    TreeObjectData() = delete;
    TreeObjectData(const TreeObjectData&) = delete;
    TreeObjectData& operator=(const TreeObjectData&) = delete;
};

// Opaque wrapper for TreeObject.
// Size is 0xD8 in both layouts. Differences:
//   selectors: re3=0x24, tdb71=0x20
//   root_node: re3=0xA0, tdb71=0xC0
class TreeObject {
public:
    bool is_delayed() const;
    void relocate(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNode>& new_nodes);
    void relocate_datas(uintptr_t old_start, uintptr_t old_end, sdk::NativeArrayNoCapacity<TreeNodeData>& new_nodes);

    ::REManagedObject* get_uservariable_hub() const;

    // --- Stable-offset accessors ---
    sdk::behaviortree::TreeObjectData* get_data() const {
        return *(sdk::behaviortree::TreeObjectData**)((uintptr_t)this + 0x8);
    }

    // Node array: pointer at 0x10, count at 0x18 (read as uint32_t, correct on LE for both)
    TreeNode* get_nodes_ptr() const {
        return *(TreeNode**)((uintptr_t)this + 0x10);
    }

    uint32_t get_node_count() const {
        return *(uint32_t*)((uintptr_t)this + 0x18);
    }

    sdk::NativeArray<::REManagedObject*>& get_action_array() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x40);
    }

    sdk::NativeArray<::REManagedObject*>& get_condition_array() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x50);
    }

    sdk::NativeArray<::REManagedObject*>& get_transition_array() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x60);
    }

    // delayed arrays at stable offsets
    sdk::NativeArray<::REManagedObject*>& get_delayed_actions() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0x90);
    }

    sdk::NativeArray<::REManagedObject*>& get_delayed_conditions() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0xA0);
    }

    sdk::NativeArray<::REManagedObject*>& get_delayed_transitions() const {
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + 0xB0);
    }

    // --- Dispatching accessors ---
    sdk::NativeArray<::REManagedObject*>& get_selector_array() const {
        const auto offset = needs_legacy_bhvt() ? 0x24 : 0x20;
        return *(sdk::NativeArray<::REManagedObject*>*)((uintptr_t)this + offset);
    }

    TreeNode* get_root_node() const {
        const auto offset = needs_legacy_bhvt() ? 0xA0 : 0xC0;
        return *(TreeNode**)((uintptr_t)this + offset);
    }

    void set_root_node(TreeNode* node) {
        const auto offset = needs_legacy_bhvt() ? 0xA0 : 0xC0;
        *(TreeNode**)((uintptr_t)this + offset) = node;
    }

    // --- Stride-aware iteration ---
    sdk::behaviortree::TreeNode* begin() const {
        if (get_node_count() == 0 || get_nodes_ptr() == nullptr) {
            return nullptr;
        }
        return get_nodes_ptr();
    }

    sdk::behaviortree::TreeNode* end() const {
        if (get_node_count() == 0 || get_nodes_ptr() == nullptr) {
            return nullptr;
        }
        return (TreeNode*)((uint8_t*)get_nodes_ptr() + get_node_count() * tree_node_stride());
    }

    // Provides the NativeArrayNoCapacity view — but callers MUST NOT iterate it
    // with pointer arithmetic; use get_node(i) or the stride helpers instead.
    sdk::NativeArrayNoCapacity<TreeNode>& get_node_array() {
        return *(sdk::NativeArrayNoCapacity<TreeNode>*)((uintptr_t)this + 0x10);
    }

    bool empty() const {
        return get_node_count() == 0 || get_nodes_ptr() == nullptr;
    }

    sdk::behaviortree::TreeNode* get_node(uint32_t index) const {
        if (index >= get_node_count() || get_nodes_ptr() == nullptr) {
            return nullptr;
        }
        return tree_node_at(get_nodes_ptr(), index);
    }

    std::vector<sdk::behaviortree::TreeNode*> get_nodes() const {
        std::vector<sdk::behaviortree::TreeNode*> out{};
        const auto count = get_node_count();
        auto* base = get_nodes_ptr();
        if (base == nullptr) return out;

        for (uint32_t i = 0; i < count; i++) {
            out.push_back(tree_node_at(base, i));
        }
        return out;
    }

    sdk::behaviortree::TreeNode* get_node_by_name(std::wstring_view name) const {
        if (this->empty()) {
            return nullptr;
        }

        const auto count = get_node_count();
        auto* base = get_nodes_ptr();
        for (uint32_t i = 0; i < count; i++) {
            auto* node = tree_node_at(base, i);
            if (node->get_name() == name) {
                return node;
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

        const auto count = get_node_count();
        auto* base = get_nodes_ptr();
        for (uint32_t i = 0; i < count; i++) {
            auto* node = tree_node_at(base, i);
            if (node->get_id() == id) {
                return node;
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
        const auto data = get_data();
        if (data == nullptr) {
            return 0;
        }
        return data->get_actions().size();
    }

    uint32_t get_static_action_count() const {
        const auto data = get_data();
        if (data == nullptr) {
            return 0;
        }
        return data->get_static_actions().size();
    }

    uint32_t get_static_condition_count() const {
        const auto data = get_data();
        if (data == nullptr) {
            return 0;
        }
        return data->get_static_conditions().size();
    }

    uint32_t get_static_transition_count() const {
        const auto data = get_data();
        if (data == nullptr) {
            return 0;
        }
        return data->get_static_transitions().size();
    }

private:
    TreeObject() = delete;
    TreeObject(const TreeObject&) = delete;
    TreeObject& operator=(const TreeObject&) = delete;
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
