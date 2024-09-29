#include "Register.hpp"

/* subject to change as the entity / component containers arent fixed yet */
component_id Registry<>::addComponent(entity_id entity, component c)
{
    const component_id comp_id = _components.size();
    _entities[entity].push_back(comp_id);
    _components[comp_id] = c;
    std::pair<component_id, entity_id> tmp{comp_id, entity};
    _registry[std::type_index(typeid(c))].push_back(tmp);
    return comp_id;
}

void Registry<>::addSystem(system sys, system_type sys_type)
{
    if (sys_type == SYSTEM_STARTUP)
        startup_systems.push_back(sys);
    else if (sys_type == SYSTEM_UPDATE)
        update_systems.push_back(sys);
    else
        std::cerr << "Invalid system type\n";
}

std::vector<system> Registry<>::getSystem(system_type type)
{
    if (type == SYSTEM_STARTUP)
        return startup_systems;
    else if (type == SYSTEM_UPDATE)
        return update_systems;
    else
        std::cerr << "Invalid system type\n";
    return update_systems;
}

// potentially add a component to all entities, done here
entity_id Registry<>::addEntity()
{
    const entity_id id = _entities.size();
    _entities[id] = entity{};
    return id;
}

template <typename cp>
cp getComponent(entity_id entity)
{
    std::vector<component_id> ids = _entities[entity];

    for (auto &id : ids)
    {
        auto current = _components[id];
        try
        {
            cp res = std::get<cp>(current);
            return res;
        }
        catch (const std::exception &e)
        {
        }
    }
    throw std::runtime_error("doesnt exist");
}

template <typename cp>
component_type getComponentType()
{
    return std::type_index(typeid(cp));
}

template <typename cp>
std::vector<component_id> getComponentIds()
{
    component_type t = getComponentType<cp>();
    std::vector<std::pair<component_id, entity_id>> fitting = _registry[t];
    std::vector<component_id> ids = std::vector<component_id>(fitting.size());

    for (size_t i = 0; i < fitting.size(); i++)
        ids[i] = fitting[i].first;
    return ids;
}

template <typename cp>
std::vector<entity_id> getEntityIds()
{
    component_type t = getComponentType<cp>();
    std::vector<std::pair<component_id, entity_id>> fitting = _registry[t];
    std::vector<entity_id> ids = std::vector<entity_id>(fitting.size());

    for (size_t i = 0; i < fitting.size(); i++)
        ids[i] = fitting[i].second;
    return ids;
}

template <typename cp>
std::vector<cp> getComponents()
{
    std::vector<component_id> ids = getComponentIds<cp>();
    std::vector<cp> res;

    res.reserve(ids.size());
    std::for_each(ids.begin(), ids.end(),
                  [&](component_id &id)
                  { res.push_back(std::get<cp>(_components[id])); });
    return res;
}