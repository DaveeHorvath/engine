#include <ostream>
#include <sstream>
#include "Components.hpp"
#include <iostream>

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

std::istream& operator>>(std::istream& is, glm::vec3& t)
{
    is >> t.x;
    is >> t.y;
    is >> t.z;
    return is;
}

std::istream& operator>>(std::istream& is, Transform& t)
{
    is >> t.pos;
    is >> t.scale;
    is >> t.rotation;
    std::cout << t;
    return is;
}

std::istream& operator>>(std::istream& is, Renderable& t)
{
    is >> t.model_name;
    std::cout << t;
    return is;
}