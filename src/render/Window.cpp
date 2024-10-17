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
}

static void framebufferResizeCallback(GLFWwindow *win, int height, int width)
{
    auto app = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(win));
    app->frameResize = true;
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
            std::cout << Logger::info << "keyboard A" << Logger::reset;
            Renderer::cam.pos.x -= 0.1;
            break;
        case GLFW_KEY_W:
            std::cout << Logger::info << "keyboard W" << Logger::reset;
            Renderer::cam.pos.y += 0.1;
            break;
        case GLFW_KEY_S:
            std::cout << Logger::info << "keyboard S" << Logger::reset;
            Renderer::cam.pos.y -= 0.1;
            break;
        case GLFW_KEY_D:
            std::cout << Logger::info << "keyboard D" << Logger::reset;
            Renderer::cam.pos.x += 0.1;
            break;
        case GLFW_KEY_Q:
            std::cout << Logger::info << "keyboard Q" << Logger::reset;
            Renderer::cam.pos.z += 0.1;
            break;
        case GLFW_KEY_E:
            std::cout << Logger::info << "keyboard E" << Logger::reset;
            Renderer::cam.pos.z -= 0.1;
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        default:
            break;
        }
    }
}