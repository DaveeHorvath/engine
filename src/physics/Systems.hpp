#ifndef PHYSICS_SYSTEM
#define PHYSICS_SYSTEM

struct Transform;
struct Rigidbody;
class Constraint;

#include "Register.hpp"

class PhysicsSystem {
    Registry<Transform, Rigidbody> physicsRegister;
    static PhysicsSystem* instance;
    std::vector<Constraint> constraints;
    PhysicsSystem();
};

#endif