#ifndef HIERARCHYWINDOW_HPP
#define HIERARCHYWINDOW_HPP

#include "Components.hpp"
#include "Register.hpp"
#include "imgui.h"
#include "ImGuizmo.h"
class HierarchyWindow{
public:
    void show();
    void init();
    void showTree(TreeNode& from);
};


#endif