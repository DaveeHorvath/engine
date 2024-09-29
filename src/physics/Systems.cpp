#include "Systems.hpp"

PhysicsSystem::PhysicsSystem()
{
    if (instance == nullptr)
        instance = this;
    else
        throw std::runtime_error("multiple instances of physics");
}