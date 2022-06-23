#pragma once

#include <cstdint>
#include <vector>

#include "REString.hpp"

#if TDB_VER >= 69
#include "regenny/re2_tdb70/via/behaviortree/BehaviorTreeCoreHandleArray.hpp"
#include "regenny/re2_tdb70/via/motion/MotionFsm2Layer.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeNodeData.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeNode.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeObject.hpp"
#include "regenny/re2_tdb70/via/behaviortree/BehaviorTree.hpp"
#else
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
class TreeNodeData;
class TreeNode;
class TreeObject;

class TreeNodeData : public regenny::via::behaviortree::TreeNodeData {
public:
};

class TreeNode : public regenny::via::behaviortree::TreeNode {
public:
    std::vector<TreeNode*> get_children() const;
    std::vector<TreeNode*> get_actions() const;
    std::vector<TreeNode*> get_transitions() const;

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

    regenny::via::behaviortree::Selector* get_selector() const {
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

    std::wstring get_full_name() const {
        auto out = std::wstring{this->get_name()};
        auto cur = this->get_parent();
        
        while (cur != nullptr && cur->get_name() != L"root" && cur != this) {
            out = std::wstring{cur->get_name()} + L"." + out;
            cur = cur->get_parent();
        }

        return out;
    }
};

class TreeObject : public regenny::via::behaviortree::TreeObject {
public:
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
};

class CoreHandle : public regenny::via::behaviortree::CoreHandle {
public:
    sdk::behaviortree::TreeObject* get_tree_object() const {
        return (sdk::behaviortree::TreeObject*)this->core.tree_object;
    }
};
};

class MotionFsm2Layer : public regenny::via::motion::MotionFsm2Layer {
public:
    sdk::behaviortree::TreeObject* get_tree_object() const {
        return (sdk::behaviortree::TreeObject*)this->core.tree_object;
    }
};
}