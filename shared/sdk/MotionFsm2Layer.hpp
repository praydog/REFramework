#pragma once

#include <cstdint>

#include "REString.hpp"

#include "regenny/re2_tdb70/via/behaviortree/BehaviorTreeCoreHandleArray.hpp"
#include "regenny/re2_tdb70/via/motion/MotionFsm2Layer.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeNode.hpp"
#include "regenny/re2_tdb70/via/behaviortree/TreeObject.hpp"
#include "regenny/re2_tdb70/via/behaviortree/BehaviorTree.hpp"

namespace sdk {
namespace behaviortree {
class TreeNode : public regenny::via::behaviortree::TreeNode {
public:
    std::wstring_view get_name() const {
        const auto& str = *(::REString*)&this->name;

        return utility::re_string::get_view(str);
    }
};
};

class MotionFsm2Layer : public regenny::via::motion::MotionFsm2Layer {
public:
    sdk::behaviortree::TreeNode* begin() const {
        const auto tree_object = this->core.tree_object;

        if (tree_object == nullptr) {
            return nullptr;
        }

        if (tree_object->nodes.count <= 0 || tree_object->nodes.nodes == nullptr) {
            return nullptr;
        }

        return (sdk::behaviortree::TreeNode*)&tree_object->nodes.nodes[0];
    }

    sdk::behaviortree::TreeNode* end() const {
        const auto tree_object = this->core.tree_object;

        if (tree_object == nullptr) {
            return nullptr;
        }

        if (tree_object->nodes.count <= 0 || tree_object->nodes.nodes == nullptr) {
            return nullptr;
        }

        return (sdk::behaviortree::TreeNode*)&tree_object->nodes.nodes[tree_object->nodes.count];
    }

    bool empty() const {
        const auto tree_object = this->core.tree_object;

        if (tree_object == nullptr) {
            return true;
        }

        if (tree_object->nodes.count <= 0 || tree_object->nodes.nodes == nullptr) {
            return true;
        }

        return false;
    }

    sdk::behaviortree::TreeNode* get_node(uint32_t index) const {
        const auto tree_object = this->core.tree_object;

        if (tree_object == nullptr) {
            return nullptr;
        }

        if (tree_object->nodes.count <= index || tree_object->nodes.nodes == nullptr) {
            return nullptr;
        }

        return (sdk::behaviortree::TreeNode*)&tree_object->nodes.nodes[index];
    }

    uint32_t get_node_count() const {
        const auto tree_object = this->core.tree_object;

        if (tree_object == nullptr) {
            return 0;
        }

        return tree_object->nodes.count;
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
}