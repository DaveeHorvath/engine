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

toml::table& save(toml::table& location, const Renderable& renderable)
{
    location.emplace("model_name", renderable.model_name);
    return location;
}

toml::table& read(toml::table& location, Renderable& renderable)
{
    renderable.model_name = **location.get_as<std::string>("model_name");
    return location;
}

toml::table& save(toml::table& location, const Transform& transform)
{
    location.emplace("pos", toml::array{"x", transform.pos.x, "y", transform.pos.y, "z", transform.pos.z});
    location.emplace("scale", toml::array{"x", transform.scale.x, "y", transform.scale.y, "z", transform.scale.z});
    location.emplace("rotation", toml::array{"x", transform.rotation.x, "y", transform.rotation.y, "z", transform.rotation.z});
    return location;
}

toml::table& read(toml::table& location, Transform& transform)
{
    auto pos = location.get_as<toml::array>("pos");

    transform.pos.x = *pos->get_as<float>(0);
    transform.pos.y = *pos->get_as<float>(1);
    transform.pos.z = *pos->get_as<float>(2);

    auto scale = location.get_as<toml::array>("scale");

    transform.scale.x = *scale->get_as<float>(0);
    transform.scale.y = *scale->get_as<float>(1);
    transform.scale.z = *scale->get_as<float>(2);

    auto rotation = location.get_as<toml::array>("rotation");

    transform.rotation.x = *rotation->get_as<float>(0);
    transform.rotation.y = *rotation->get_as<float>(1);
    transform.rotation.z = *rotation->get_as<float>(2);
    return location;
}