#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WIDTH 2000
#define HEIGHT 1200

static void framebufferResizeCallback(GLFWwindow * win, int height, int width);
static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class Window {
public:
    Window();
    ~Window();
    void init();
    inline static GLFWwindow *win;
};

#endif