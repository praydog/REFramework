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
}
}