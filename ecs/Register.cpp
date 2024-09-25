#include <variant>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <iostream>
#include <typeindex>
#include <typeinfo>

#define SYSTEM_UPDATE 0
#define SYSTEM_STARTUP 1
namespace DHecs
{

	template <typename... cmps>
	class Registry
	{
		using component_type = std::type_index;
		using component_id = uint32_t;
		std::variant<cmps...> component;

		// system should expand to have fixed data members
		// for faster queries
		using system = void(*)();
		using system_type = char;

		using entity = std::vector<component_id>;
		using entity_id = uint32_t;

	public:
		template <typename cp>
		component_type getComponentType()
		{
			return std::type_index(typeof(cp));
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
			std::for_each(ids.begin(), ids.end(), [&](component_id &id) { res.push_back(_components.find(id)); });
			return res;
		}

		template <typename cp>
		cp getComponent(entity_id entity)
		{
			std::vector<component_id> ids = _entities[entity];

			return std::find(ids.begin(), ids.end(), [&](component_id &id) { return std::type_index(typeof(cp)) == std::type_index(typeof(_components[id])); });
		}

		/* subject to change as the entity / component containers arent fixed yet */
		component_id addComponent(entity_id entity, component c)
		{
			const component_id comp_id = _components.size();
			_entities[entity].push_back(comp_id);
			_components.insert(comp_id, c);
			std::pair<component_id, entity_id> tmp{comp_id, entity};
			_registry[std::type_index(typeof(c))].push_back(tmp);
			return comp_id;
		}

		void addSystem(system sys, system_type sys_type)
		{
			if (sys_type == SYSTEM_STARTUP)
				startup_systems.push_back(sys);
			else if (sys_type == SYSTEM_UPDATE)
				update_systems.push_back(sys);
			else
				std::cerr << "Invalid system type\n";
		}

		std::vector<system> getSystem(system_type type)
		{
			if (sys_type == SYSTEM_STARTUP)
				return startup_systems;
			else if (sys_type == SYSTEM_UPDATE)
				return update_systems;
			else
				std::cerr << "Invalid system type\n";
			return update_systems;
		}

		// potentially add a component to all entities, done here
		entity_id addEntity()
		{
			const entity_id id = _entities.size();
			_entities[id] = entity{};
			return id;
		}

	private:
		std::unordered_map<component_type, std::vector<std::pair<component_id, entity_id>>> _registry;
		std::unordered_map<component_id, component> _components;
		std::unordered_map<entity_id, entity> _entities;

		std::vector<system> startup_systems;
		std::vector<system> update_systems;
	};
};

struct Pos {
	int x;
	int y;
	int z;
};

struct Velocity {
	int x;
	int y;
	int z;
};

// this app would be created from a scope where all systems will be run and accesed anyway
// it also needs deltatime for most applications and systems
DHecs::Registry<Pos, Velocity> reg;

void GravitySystem()
{
	auto vel = reg.getComponents<Velocity>();
	for (auto& v : vel)
	{
		v.y -= 10;
	}
}

/* bad demo, queries arent that good yet */
void VelocitySystem()
{
	auto eids = reg.getEntityIds<Velocity>();
	for (auto& id : eids)
	{
		Pos p = reg.getComponent<Pos>(id);
		Velocity v = reg.getComponent<Velocity>(id);
		p.x += v.x;
		p.y += v.y;
		p.z += v.z;
	}
}

/* more of a scene setup function */
int main()
{
	reg.addSystem(GravitySystem, SYSTEM_UPDATE);
	reg.addSystem(VelocitySystem, SYSTEM_UPDATE);

	uint32_t en = reg.addEntity();
	Pos en_pos = {0,0,0};
	Pos en_vel = {0,0,0};

	reg.addComponent(en, en_pos);
	reg.addComponent(en, en_vel);

}