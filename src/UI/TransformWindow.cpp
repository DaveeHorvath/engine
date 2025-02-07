#include "TransformWindow.hpp"

#include <imgui_internal.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Renderer.hpp"

// #include "ImGuizmo.h"

void TransformWindow::show()
{
    // Window
    ImGui::Begin("TransformTool", &isActive);
    ImGui::Text("Transform #%d", current);

    ImGui::DragFloat3("position", glm::value_ptr(target->pos), 0.5);
    ImGui::DragFloat3("scale", glm::value_ptr(target->scale), 0.2, 0.0001f);
    ImGui::DragFloat3("rotation", glm::value_ptr(target->rotation), 1);

    if (ImGui::RadioButton("Translate", op == ImGuizmo::OPERATION::TRANSLATE))
        op = ImGuizmo::OPERATION::TRANSLATE;
    if (ImGui::RadioButton("Rotate", op == ImGuizmo::OPERATION::ROTATE))
        op = ImGuizmo::OPERATION::ROTATE;
    if (ImGui::RadioButton("Scale", op == ImGuizmo::OPERATION::SCALE))
        op = ImGuizmo::OPERATION::SCALE;
    if (ImGui::RadioButton("Universal", op == ImGuizmo::OPERATION::UNIVERSAL))
        op = ImGuizmo::OPERATION::UNIVERSAL;

    if (ImGui::Button("Edit"))
        isRunning = true;
    if (ImGui::Button("Run"))
        isRunning = false;

    if (!isRunning)
        ImGui::DragFloat("Speed", &playerSpeed, 0.2);

    if (ImGui::Button("spawn"))
        renderer->spawnCube();

    ImGui::End();
}

void TransformWindow::init(Renderer *r)
{
    target = g_reg.getComponents<Transform>()[current];
    renderer = r;
}

void TransformWindow::updateTarget()
{
    auto ref = g_reg.getComponents<Transform>();

    current = (current + 1) % ref.size();
    std::cout << Logger::info << "Selecting transform #" << current << Logger::reset;
    target = ref[current];
}

void TransformWindow::updateTarget(const int& id)
{
    auto ref = g_reg.getComponents<Transform>();

    current = id % ref.size();
    std::cout << Logger::info << "Selecting transform #" << current << Logger::reset;
    target = ref[current];
}