#ifndef VIOLET_Engine_H
#define VIOLET_Engine_H

#include "engine/Defines.h"
#include "engine/entity/Entity.h"
#include "engine/scene/Scene.h"
#include "engine/system/System.h"
#include "engine/utility/task/TaskScheduler.h"
#include "engine/utility/task/Task.h"

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

		class VIOLET_API Task : public Violet::Task
		{
		public:

			Task(const Engine & engine, uint32 priority = ms_defaultPriority);
			virtual ~Task() override = default;

		protected:

			Engine & m_engine;
		};

	public:

		static std::unique_ptr<Engine> init(SystemFactory & factory, Deserializer & deserializer);

	public:

		void begin();
		void runFrame(float frameTime);
		void switchScene(const char * filename);
		Scene & getCurrentScene();
		const Scene & getCurrentScene() const;
		void stop();

		template <typename SystemType>
		const std::unique_ptr<SystemType> & getSystem();
		template <typename SystemType>
		const std::unique_ptr<const SystemType> & getSystem() const;

		void addTask(std::unique_ptr<Task> && task) const;

	private:

		Engine(std::vector<std::unique_ptr<System>> && systems, std::unique_ptr<Scene> && scene, uint32 workerCount);

		Engine(const Engine &) = delete;
		Engine & operator=(const Engine &) = delete;

	private:

		std::string m_nextSceneFileName;
		std::vector<std::unique_ptr<System>> m_systems;
		std::unique_ptr<Scene> m_scene;
		mutable TaskScheduler m_taskScheduler;
		bool m_running;
	};
}

#include "engine/Engine.inl"

#endif