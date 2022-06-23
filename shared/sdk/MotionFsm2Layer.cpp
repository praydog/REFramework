#include "RETypeDB.hpp"

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

std::vector<TreeNode*> TreeNode::get_actions() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<TreeNode*> out{};

    for (auto i = 0; i < tree_data->actions.count; ++i) {
        const auto action_index = tree_data->actions.data[i];

        auto node = tree_owner->get_node(action_index);

        if (node != nullptr) {
            out.push_back(node);
        }
    }

    return out;
}

std::vector<TreeNode*> TreeNode::get_transitions() const {
    const auto tree_data = get_data();

    if (tree_data == nullptr) {
        return {};
    }

    const auto tree_owner = this->get_owner();
    
    if (tree_owner == nullptr) {
        return {};
    }
    
    std::vector<TreeNode*> out{};

    for (auto i = 0; i < tree_data->transitions.count; ++i) {
        const auto transition_index = tree_data->transitions.data[i];

        auto node = tree_owner->get_node(transition_index);

        if (node != nullptr) {
            out.push_back(node);
        }
    }

    return out;
}

void BehaviorTree::set_current_node(sdk::behaviortree::TreeNode* node, uint32_t tree_idx, void* set_node_info) {
    static auto method = sdk::find_method_definition("via.behaviortree.BehaviorTree", "setCurrentNode(System.UInt64, System.UInt32, via.behaviortree.SetNodeInfo)");

    if (method == nullptr) {
        return;
    }

    method->call(sdk::get_thread_context(), this, node->get_id(), tree_idx, set_node_info);
}
}
}