#include "Window.hpp"
#include <iostream>
#include "Renderer.hpp"
#include "Logger.hpp"

Window::Window()
{
    init();
}

Window::~Window()
{
    glfwDestroyWindow(win);
    glfwTerminate();
}

void Window::init()
{
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    win = glfwCreateWindow(WIDTH, HEIGHT, "FUNny", nullptr, nullptr);
    glfwSetWindowUserPointer(win, this);
    glfwSetFramebufferSizeCallback(win, framebufferResizeCallback);
    glfwSetKeyCallback(win, keyboardCallback);
    glfwSetCursorPosCallback(win, mouseCallback);
}

static void framebufferResizeCallback(GLFWwindow *win, int height, int width)
{
    auto app = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(win));
    app->frameResize = true;
}

static void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !ImGuizmo::IsUsingAny())
    {
 //       std::cout << Logger::info << ypos - Window::prevY << Logger::reset;
 //       std::cout << Logger::info << xpos - Window::prevX << Logger::reset;
        // resolutin agnostic version
       Renderer::cam.pitch += glm::degrees(glm::asin((ypos - Window::prevY) / HEIGHT * 2));
       Renderer::cam.tilt -= glm::degrees(glm::asin((xpos - Window::prevX) / WIDTH * 2));
       if (Renderer::cam.pitch <= -90)
           Renderer::cam.pitch = -89.99;
       if (Renderer::cam.pitch >= 90)
           Renderer::cam.pitch = 89.99;
    }

    Window::prevX = xpos;
    Window::prevY = ypos;
}

static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT || action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_P:
            TransformWindow::updateTarget();
            break;
        case GLFW_KEY_A:
            // std::cout << Logger::info << "keyboard A" << Logger::reset;
            if (TransformWindow::isRunning)
                Renderer::cam.pos.x -= .5;
            else
                Window::isA = true;
            break;
        case GLFW_KEY_W:
            // std::cout << Logger::info << "keyboard W" << Logger::reset;
            if (TransformWindow::isRunning)
                Renderer::cam.pos.y += .5;
            else
                Window::isW = true;
            break;
        case GLFW_KEY_S:
            // std::cout << Logger::info << "keyboard S" << Logger::reset;
            if (TransformWindow::isRunning)
                Renderer::cam.pos.y -= .5;
            else
                Window::isS = true;
            break;
        case GLFW_KEY_D:
            // std::cout << Logger::info << "keyboard D" << Logger::reset;
            if (TransformWindow::isRunning)
                Renderer::cam.pos.x += .5;
            else
                Window::isD = true;
            break;
        case GLFW_KEY_Q:
            // std::cout << Logger::info << "keyboard Q" << Logger::reset;
            if (TransformWindow::isRunning)
                Renderer::cam.pos.z += .5;
            break;
        case GLFW_KEY_E:
            // std::cout << Logger::info << "keyboard E" << Logger::reset;
            if (TransformWindow::isRunning)
                Renderer::cam.pos.z -= .5;
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        default:
            break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch (key)
        {
        case GLFW_KEY_A:
            // std::cout << Logger::info << "keyboard A" << Logger::reset;
                Window::isA = false;
            break;
        case GLFW_KEY_W:
            // std::cout << Logger::info << "keyboard W" << Logger::reset;
                Window::isW = false;
            break;
        case GLFW_KEY_S:
            // std::cout << Logger::info << "keyboard S" << Logger::reset;
                Window::isS = false;
            break;
        case GLFW_KEY_D:
            // std::cout << Logger::info << "keyboard D" << Logger::reset;
                Window::isD = false;
            break;
        }
    }
}