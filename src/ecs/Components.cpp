#include <ostream>
#include <sstream>
#include "Components.hpp"
#include <iostream>

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

    transform.pos.x = pos->get_as<double>(0)->value_or(0.0);
    transform.pos.y = pos->get_as<double>(1)->value_or(0.0);
    transform.pos.z = pos->get_as<double>(2)->value_or(0.0);

    auto scale = location.get_as<toml::array>("scale");

    transform.scale.x = scale->get_as<double>(0)->value_or(1.0);
    transform.scale.y = scale->get_as<double>(1)->value_or(1.0);
    transform.scale.z = scale->get_as<double>(2)->value_or(1.0);

    auto rotation = location.get_as<toml::array>("rotation");

    transform.rotation.x = rotation->get_as<double>(0)->value_or(0.0);
    transform.rotation.y = rotation->get_as<double>(1)->value_or(0.0);
    transform.rotation.z = rotation->get_as<double>(2)->value_or(0.0);
    return location;
}