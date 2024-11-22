#ifndef REGISTRY_HPP
# define REGISTRY_HPP

#include <variant>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <algorithm>

#include "Components.hpp"
#include "Logger.hpp"
#include "Tree.hpp"

#include <fstream>
#include <sstream>

#define SYSTEM_UPDATE 0
#define SYSTEM_STARTUP 1
#include "toml.hpp"

template <typename... cmps>
class Registry
{
public:
    using component_type = std::type_index;
    using component_id = uint32_t;

    using entity = std::vector<component_id>;
    using entity_id = uint32_t;
    using component = std::variant<cmps...>;

    template <typename cp>
    component_type getComponentType()
    {
        return std::type_index(typeid(cp));
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
    std::vector<cp*> getComponents()
    {
        std::vector<component_id> ids = getComponentIds<cp>();
        std::vector<cp*> res;

        res.reserve(ids.size());
        std::for_each(ids.begin(), ids.end(),
                    [&](component_id &id)
                    { try {res.emplace_back(&std::get<cp>(_components[id]));} catch (...){} });
        return res;
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

    std::vector<component_id> getComponentIds(entity_id entity)
    {
        return _entities[entity];
    }

    template<typename cp>
    component_id addComponent(entity_id entity, cp c)
    {
        const component_id comp_id = _components.size();
        _entities[entity].push_back(comp_id);
        _components[comp_id] = c;
        // std::pair<component_id, entity_id> tmp{comp_id, entity};
        _registry[std::type_index(typeid(cp))].emplace_back(comp_id, entity);
        return comp_id;
    }

    entity_id addEntity()
    {
        const entity_id id = _entities.size();
        std::cout << Logger::info <<"entity #" << id << " added to scene" << Logger::reset;
        _entities[id] = entity{};
        scene.children.push_back((TreeNode){.cont=id});
        return id;
    }

    // TODO add hinted parent with TreeNode&
    entity_id addEntity(const entity_id& parent)
    {
        const entity_id id = _entities.size();
        std::cout << Logger::info <<"entity #" << id << " added to scene" << Logger::reset;
        _entities[id] = entity{};
        // really slow probably
        TreeNode *parent_node = scene.findChild(parent);
        if (parent_node != nullptr)
        {
            parent_node->children.push_back((TreeNode){.cont=id});
            std::cout << Logger::debug << "found parent" << Logger::reset;
        }
        else
        {
            std::cout << Logger::debug << "didnt find parent" << Logger::reset;
            scene.children.push_back((TreeNode){.cont=id});
        }
        return id;
    }

    void loadScene()
    {

    }

    void saveScene()
    {
        
    }

    // TODO make private and create accessors
    TreeNode scene{};
private:
    std::unordered_map<component_type, std::vector<std::pair<component_id, entity_id>>> _registry{};
    std::unordered_map<component_id, component> _components{};
    std::unordered_map<entity_id, entity> _entities{};
};

// ADD ALL COMPONENTS HERE
extern Registry<Transform, Renderable> g_reg;

#endif