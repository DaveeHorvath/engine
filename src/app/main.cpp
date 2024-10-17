#include <iostream>
#include "Logger.hpp"
#include <chrono>
#include <ostream>

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
        // scene loading
        // g_reg.loadScene();

        uint32_t first = g_reg.addEntity();
        Transform t;
        t.pos = {0,0,1};
        t.scale = {12,12,12};
        t.rotation = {0,0,-180};
        Renderable r;
        r.model_name = "resources/smooth_vase.obj";

        g_reg.addComponent(first, t);
        g_reg.addComponent(first, r);

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
        // g_reg.saveScene();
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
    // t->scale.y += 0.3 * deltaTime;
    Transform* t2 = g_reg.getComponents<Transform>()[1];
    // t2->pos.y -= 0.3 * deltaTime;
}