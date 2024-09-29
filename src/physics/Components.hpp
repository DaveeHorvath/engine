#ifndef PHYSICS_COMPONENTS
#define PHYSICS_COMPONENTS

#include <glm/glm.hpp>

struct Transform {
    glm::vec3 pos;
    glm::vec3 scale;
    glm::vec3 rotation;
};

struct Rigidbody
{
    glm::vec3 velocity;
    glm::vec3 force; // acceleration
};

#endif