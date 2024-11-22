#include "HierarchyWindow.hpp"
#include <imgui_internal.h>
#include <imgui.h>
#include "Register.hpp"

void HierarchyWindow::show()
{
    // Window
    ImGui::Begin("Hierarchy");
    ImGui::Text("Scene");
    for (auto& child : g_reg.scene.children)
        showTree(child);
    ImGui::End();
}

void HierarchyWindow::init()
{}

void HierarchyWindow::showTree(TreeNode &from)
{
    if (from.children.size() == 0)
    {
        if (ImGui::Selectable(std::to_string(from.cont).c_str(), ImGuiSelectableFlags_Highlight))
            std::cout << Logger::info << "Clicked entity #" << from.cont << Logger::reset;
    }
    else
    {
        if (ImGui::TreeNode(std::to_string(from.cont).c_str()))
        {
            if (ImGui::Selectable(std::to_string(from.cont).c_str()))
                std::cout << Logger::info << "Clicked entity #" << from.cont << Logger::reset;
            for (auto& child : from.children)
                showTree(child);
            ImGui::TreePop();
        }
    }
}
