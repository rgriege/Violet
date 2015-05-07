#ifndef VIOLET_Engine_H
#define VIOLET_Engine_H

#include "engine/Defines.h"
#include "engine/entity/Entity.h"
#include "engine/entity/EntityManager.h"
#include "engine/scene/Scene.h"
#include "engine/system/System.h"

#include <memory>
#include <vector>

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
		Scene & getCurrentScene();
		void stop();

		template <typename SystemType>
		SystemType * fetch();

	private:

		Engine(std::vector<std::unique_ptr<System>> && systems, Scene && scene);

		Engine(const Engine &) = delete;
		Engine & operator=(const Engine &) = delete;

	private:

		std::string m_nextSceneFileName;
		std::vector<std::unique_ptr<System>> m_systems;
		Scene m_scene;
		bool m_running;
	};
}

#include "engine/Engine.inl"

#endif