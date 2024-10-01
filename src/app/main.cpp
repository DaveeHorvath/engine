#include <iostream>
#include "Logger.hpp"
#include <chrono>

// components
#include "Register.hpp"
#include "Components.hpp"

// systems
#include "Renderer.hpp"

void UpdateTransform(float deltaTime);
Registry<Transform, Renderable> g_reg;

int main()
{
    // systems
    Renderer renderer;

    // time
    // parse scene file for registry entities
        // Todo
    // runtime
    try
    {
        std::cout << "\033[2J";
        // should make a time class for all these
        uint32_t tea1 = g_reg.addEntity();
        Transform tea1_transform = {{0,-2,0}, {1,1,1}, {1,1,1}};
        Renderable tea1_renderable = {"resources/teapot.obj"};
        g_reg.addComponent(tea1, tea1_transform);
        g_reg.addComponent(tea1, tea1_renderable);


        uint32_t tea2 = g_reg.addEntity();
        Transform tea2_transform = {{0,2,0}, {1,1,1}, {1,1,1}};
        Renderable tea2_renderable = {"resources/teapot.obj"};
        g_reg.addComponent(tea2, tea2_transform);
        g_reg.addComponent(tea2, tea2_renderable);

        renderer.init();
        const auto startTime = std::chrono::high_resolution_clock::now();
        std::cout << Logger::info << "Main loop" << Logger::reset;
        auto last = std::chrono::high_resolution_clock::now();
        while (!glfwWindowShouldClose(Window::win))
        {
            auto current = std::chrono::high_resolution_clock::now();
            float deltatime = std::chrono::duration<float, std::chrono::seconds::period>(current - last).count();
            glfwPollEvents();
            // run update for every system
            UpdateTransform(deltatime);
            last = current;
            renderer.drawFrame();
        }
        std::cout << Logger::info << "Terminating" << Logger::reset;
        renderer.clean();
    }
    catch(const std::exception& e)
    {
        std::cerr << "\033[1;31" << e.what() << "\033[0m" << '\n';
        exit(69);
    }
}

void UpdateTransform(float deltaTime)
{
    Transform* t = g_reg.getComponents<Transform>()[0];
    t->pos.y += 0.3 * deltaTime;
    Transform* t2 = g_reg.getComponents<Transform>()[1];
    t2->pos.y -= 0.3 * deltaTime;
}