#ifndef DEFAULT_COMPONENTS
#define DEFAULT_COMPONENTS

#include <memory>
#include <glm/glm.hpp>
#include <string>
struct Transform {
    glm::vec3 pos;
    glm::vec3 scale;
    glm::vec3 rotation;
};

struct Renderable {
    std::string model_name;
};

#endif