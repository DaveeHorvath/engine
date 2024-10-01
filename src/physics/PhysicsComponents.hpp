#ifndef PHYSICS_COMPONENTS
#define PHYSICS_COMPONENTS

#include <glm/glm.hpp>

struct Rigidbody
{
    glm::vec3 velocity;
    glm::vec3 force; // acceleration
};

#endif