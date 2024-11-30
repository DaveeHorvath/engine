#ifndef TRANSFORMWINDOW_HPP
#define TRANSFORMWINDOW_HPP

#include "Components.hpp"
#include "Register.hpp"
#include "imgui.h"
#include "ImGuizmo.h"
class TransformWindow{
public:
    inline static int current = 0;
    bool isActive = true;
    inline static ImGuizmo::OPERATION op = ImGuizmo::OPERATION::TRANSLATE;
    inline static Transform* target;
    inline static bool isRunning = false;
    inline static float playerSpeed = 1;
    void show();
    void init();
    static void updateTarget();
    static void updateTarget(const int& id);
};


#endif