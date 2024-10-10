#include "TransformWindow.hpp"

#include <imgui_internal.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void TransformWindow::show()
{
    ImGui::Begin("TransformTool", &isActive);
    ImGui::Text("Transform #%d", current);

    ImGui::DragFloat3("position", glm::value_ptr(target->pos), 0.5);
    ImGui::DragFloat3("scale", glm::value_ptr(target->scale), 0.2, 0.0001f);
    ImGui::DragFloat3("rotation", glm::value_ptr(target->rotation), 1);

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