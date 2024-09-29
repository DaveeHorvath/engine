#ifndef PHYSICS_SYSTEM
#define PHYSICS_SYSTEM
#include "Components.hpp"
#include "Constraint.hpp"

#include "Register.hpp"

class PhysicsSystem {
    Registry<Transform, Rigidbody> physicsRegister;
    inline static PhysicsSystem* instance;
    std::vector<Constraint> constraints;
    PhysicsSystem();
};

#endif