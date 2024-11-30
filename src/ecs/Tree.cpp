#include "Tree.hpp"
#include <cstdint>

TreeNode* TreeNode::findChild(uint32_t val)
{
    if (this->cont == val)
        return this;
    if (this->children.size() == 0)
        return nullptr;
    for (auto& child : this->children)
    {
        TreeNode* res = child.findChild(val);
        if (res != nullptr)
            return res;
    }
    return nullptr;
}