#ifndef DEFAULT_COMPONENTS
#define DEFAULT_COMPONENTS

#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <ostream>
#include <sstream>
#include "toml.hpp"
struct Transform {
    glm::vec3 pos;
    glm::vec3 scale;
    glm::vec3 rotation;
};

struct Renderable {
    std::string model_name;
};

toml::array save(const Renderable& renderable);
toml::table save(const Transform& renderable);

#endif