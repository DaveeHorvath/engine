#include "TransformWindow.hpp"

#include <imgui_internal.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// #include "ImGuizmo.h"

void TransformWindow::show()
{
    // Window
    ImGui::Begin("TransformTool", &isActive);
    ImGui::Text("Transform #%d", current);

    ImGui::DragFloat3("position", glm::value_ptr(target->pos), 0.5);
    ImGui::DragFloat3("scale", glm::value_ptr(target->scale), 0.2, 0.0001f);
    ImGui::DragFloat3("rotation", glm::value_ptr(target->rotation), 1);

    // if (!isTranslate && !isRotate && !isScale)
    //     ImGuizmo::Enable(false);
    // else
    // {
    //     ImGuizmo::Enable(true);
    //     op = op | isTranslate ? ImGuizmo::OPERATION::TRANSLATE : op;
    //     op = op | isRotate ? ImGuizmo::OPERATION::ROTATE : op;
    //     op = op | isScale ? ImGuizmo::OPERATION::SCALE : op;
    // }

    ImGui::End();
}

void TransformWindow::init()
{
    target = g_reg.getComponents<Transform>()[current];
}

void TransformWindow::updateTarget()
{
    auto ref = g_reg.getComponents<Transform>();

    current = (current + 1) % ref.size();
    std::cout << Logger::info << "Selecting transform #" << current << Logger::reset;
    target = ref[current];
}