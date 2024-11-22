#include <iostream>
#include "Logger.hpp"
#include <chrono>
#include <ostream>

// components
#include "Register.hpp"
#include "Components.hpp"
#include "Tree.hpp"
// systems
#include "Renderer.hpp"

void UpdateTransform(float deltaTime);
Registry<Transform, Renderable> g_reg;

int main()
{
    // systems
    Renderer renderer;
    // time
    // TODO parse scene file for registry entities
    // runtime
    try
    {
        // TODO skip assign and just dont allow top level value like a None entity
        g_reg.scene.cont = 2147483648;
        std::cout << "\033[2J";
        // should make a time class for all these
        // scene loading
        // g_reg.loadScene();

        // uint32_t player = g_reg.addEntity();
        // Transform t;
        // t.pos = {0, 0, 0};
        // t.scale = {1,1,1};
        // t.rotation = {0,0,0};
        // Renderable r;
        // r.model_name = "resources/12221_Cat_v1_l3.obj";
        // g_reg.addComponent(player, t);
        // g_reg.addComponent(player, r);

        uint32_t parent = g_reg.addEntity();
        Transform t;
        t.pos = { 0,0,0};
        t.scale = {5,5,5};
        t.rotation = {0,0,-180};
        Renderable r;
        r.model_name = "resources/quad.obj";

        g_reg.addComponent(parent, t);
        g_reg.addComponent(parent, r);

        for (int i = 0; i < 3; i++)
        {
            uint32_t objs = g_reg.addEntity(parent);
            Transform t;
            t.pos = { 3 * (i % 5) - 6, 0,  3 * (i / 5)};
            t.scale = {10,10,10};
            t.rotation = {0,0,-180};
            Renderable r;
            r.model_name = "resources/quad.obj";

            g_reg.addComponent(objs, t);
            g_reg.addComponent(objs, r);
        }

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
    static Transform* t = g_reg.getComponents<Transform>()[0];
    if (!TransformWindow::isRunning)
    {
        t->pos.x += Window::isD ? TransformWindow::playerSpeed * deltaTime : 0;
        t->pos.x += Window::isA ? -TransformWindow::playerSpeed * deltaTime : 0;
        t->pos.z += Window::isW ? -TransformWindow::playerSpeed * deltaTime : 0;
        t->pos.z += Window::isS ? TransformWindow::playerSpeed * deltaTime : 0;
    }
}