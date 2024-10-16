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

std::istream& operator>>(std::istream& is, glm::vec3& t);
std::istream& operator>>(std::istream& is, Transform& t);
std::istream& operator>>(std::istream& is, Renderable& t);

#endif