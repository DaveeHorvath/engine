#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <variant>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <algorithm>

#define SYSTEM_UPDATE 0
#define SYSTEM_STARTUP 1

using component_type = std::type_index;
using component_id = uint32_t;

using system = void (*)();
using system_type = char;

using entity = std::vector<component_id>;
using entity_id = uint32_t;

template <typename cp>
component_type getComponentType();

template <typename cp>
std::vector<component_id> getComponentIds();

template <typename cp>
std::vector<entity_id> getEntityIds();

template <typename cp>
std::vector<cp> getComponents();

template <typename cp>
cp getComponent(entity_id entity);

template <typename... cmps>
class Registry
{
public:
    using component = std::variant<cmps...>;
    friend component_type getComponentType();
    friend std::vector<component_id> getComponentIds();
    friend std::vector<entity_id> getEntityIds();
    friend std::vector<cp> getComponents();
    friend cp getComponent(entity_id entity);

    component_id addComponent(entity_id entity, component c);
    void addSystem(system sys, system_type sys_type);
    std::vector<system> getSystem(system_type type);
    entity_id addEntity();
private:
    std::unordered_map<component_type, std::vector<std::pair<component_id, entity_id>>> _registry;
    std::unordered_map<component_id, component> _components;
    std::unordered_map<entity_id, entity> _entities;

    std::vector<system> startup_systems;
    std::vector<system> update_systems;
};

#endif