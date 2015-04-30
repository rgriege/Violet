#ifndef ENGINE_H
#define ENGINE_H

#include "engine/Defines.h"
#include "engine/component/ComponentFactory.h"
#include "engine/entity/Entity.h"
#include "engine/entity/EntityFactory.h"
#include "engine/system/System.h"

#include <memory>
#include <vector>
#include <algorithm>

namespace Violet
{
	class Deserializer;
	class Entity;
	class System;
	class SystemFactory;

	class VIOLET_API Engine
	{
	public:

		static std::unique_ptr<Engine> init(SystemFactory & factory, Deserializer & deserializer);

	public:

		~Engine();
		Engine(Engine && other);
		Engine & operator=(Engine && other);

		void begin();
		void runFrame(float frameTime);
		void switchScene(const char * filename);
		bool saveScene(const char * filename);
		void stop();

		template <typename ComponentType>
		bool has(const Entity & entity)
		{
			auto it = std::find_if(std::begin(m_systems), std::end(m_systems), [](std::unique_ptr<System> const & system) { return system->owns(ComponentType::getLabel()); });
			return (*it)->has(ComponentType::getLabel(), entity);
		}

		template <typename ComponentType>
		typename ComponentType & fetch(const Entity & entity)
		{
			auto it = std::find_if(std::begin(m_systems), std::end(m_systems), [](std::unique_ptr<System> const & system) { return system->owns(ComponentType::getLabel()); });
			return static_cast<ComponentType &>((*it)->fetch(ComponentType::getLabel(), entity));
		}

		template <typename SystemType>
		SystemType & fetch()
		{
			auto it = std::find_if(std::begin(m_systems), std::end(m_systems), [](std::unique_ptr<System> const & system) { return system->getLabel() == SystemType::getStaticLabel(); });
			return static_cast<SystemType &>(**it);
		}

		void remove(const Entity & entity)
		{
			for (auto const & system : m_systems)
				system->remove(entity);
		}

		EntityFactory & getEntityFactory()
		{
			return m_entityFactory;
		}

		ComponentFactory & getComponentFactory()
		{
			return m_componentFactory;
		}

	private:

		Engine(std::vector<std::unique_ptr<System>> && systems);

		Engine(const Engine &) = delete;
		Engine & operator=(const Engine &) = delete;

	private:

		std::string m_nextSceneFileName;
		std::vector<std::unique_ptr<System>> m_systems;
		EntityFactory m_entityFactory;
		ComponentFactory m_componentFactory;
		bool m_running;
	};
}

#endif