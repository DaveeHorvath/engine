#include <ostream>
#include <sstream>
#include "Components.hpp"

std::ostream& operator<<(std::ostream& os, const glm::vec3& t)
{
    os << "vec3" << "\n" << t.x << "\n" << t.y << "\n" << t.z << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Transform& t)
{
    os << "Transform\n" << t.pos << t.scale << t.rotation << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Renderable& t)
{
    os << "Renderable\n" << t.model_name << "\n";
    return os;
}

std::istringstream& operator>>(std::istringstream& is, glm::vec3& t)
{
    is >> t.x;
    is >> t.y;
    is >> t.z;
    return is;
}

std::istringstream& operator>>(std::istringstream& is, Transform& t)
{
    is >> t.pos;
    is >> t.scale;
    is >> t.rotation;
    return is;
}

std::istringstream& operator>>(std::istringstream& is, Renderable& t)
{
    is >> t.model_name;
    return is;
}