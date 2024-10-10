#ifndef DEFAULT_COMPONENTS
#define DEFAULT_COMPONENTS

#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <ostream>
#include <sstream>
struct Transform {
    glm::vec3 pos;
    glm::vec3 scale;
    glm::vec3 rotation;
};

struct Renderable {
    std::string model_name;
};

std::ostream& operator<<(std::ostream& os, const glm::vec3& t);
std::ostream& operator<<(std::ostream& os, const Transform& t);
std::ostream& operator<<(std::ostream& os, const Renderable& t);

std::istringstream& operator<<(std::istringstream& is, glm::vec3& t);
std::istringstream& operator>>(std::istringstream& is, Transform& t);
std::istringstream& operator>>(std::istringstream& is, Renderable& t);

#endif