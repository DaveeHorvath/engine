#ifndef TREE_HPP
#define TREE_HPP
#include <vector>
#include <cstdint>
struct TreeNode{
    uint32_t cont;
    std::vector<TreeNode> children;
    // TODO add hinted find
    TreeNode* findChild(uint32_t val);
};

#endif