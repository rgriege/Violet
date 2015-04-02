#ifndef ENGINE_H
#define ENGINE_H

#include "violet/core/Defines.h"
#include "violet/core/system/System.h"

#include <memory>
#include <vector>
#include <algorithm>

namespace Violet
{
	class Deserializer;
	class Entity;
	class Scene;
	class System;
	class SystemFactory;

	class VIOLET_API Engine
	{
	public:

		static std::unique_ptr<Engine> init(SystemFactory & factory, Deserializer & deserializer);

	public:

		void begin();
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

		~Engine();

	private:

		Engine(std::vector<std::unique_ptr<System>> && systems, std::unique_ptr<Scene> && firstScene);

	private:

		std::unique_ptr<Scene> m_activeScene;
		std::string m_nextSceneFileName;
		std::vector<std::unique_ptr<System>> m_systems;
		bool m_running;
	};
}

#endif