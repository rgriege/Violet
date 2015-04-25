#ifndef ENGINE_H
#define ENGINE_H

#include "violet/core/Defines.h"
#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/entity/EntityFactory.h"
#include "violet/core/system/System.h"

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

		friend class SceneInitContext;

	public:

		static std::unique_ptr<Engine> init(SystemFactory & factory, Deserializer & deserializer);

	public:

		~Engine();
		Engine(Engine && other);
		Engine & operator=(Engine && other);

		void begin();
		void runFrame(float frameTime);
		void switchScene(const char * filename);
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

		EntityFactory & getEntityFactory()
		{
			return m_entityFactory;
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

	class VIOLET_API SceneInitContext
	{
	public:

		SceneInitContext(Engine & engine) :
			m_engine(engine)
		{
		}

		Entity createEntity()
		{
			static uint32 s_id = 10000;
			return Entity(++s_id);
		}

		void createEntity(const char * label, Deserializer & deserializer)
		{
			m_engine.m_entityFactory.create(label, deserializer, *this);
		}

		void createComponent(const char * label, Entity & entity, Deserializer & deserializer)
		{
			m_engine.m_componentFactory.create(label, entity, deserializer);
		}

		template <typename SystemType, typename ComponentType, typename... Args>
		void createComponent(Entity & entity, Args&&... args)
		{
			m_engine.fetch<SystemType>().create<ComponentType>(entity, std::forward<Args>(args)...);
		}

	private:

		Engine & m_engine;
	};
}

#endif