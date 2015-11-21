#ifndef VIOLET_Engine_H
#define VIOLET_Engine_H

#include "violet/Defines.h"
#include "violet/entity/Entity.h"
#include "violet/event/EventContextOwner.h"
#include "violet/scene/Scene.h"
#include "violet/system/System.h"
#include "violet/task/TaskScheduler.h"
#include "violet/task/Task.h"

#include <memory>
#include <vector>

namespace Violet
{
	class Deserializer;
	class Entity;
	class System;
	class SystemFactory;

	class VIOLET_API Engine : public EventContextOwner
	{
	private:

		template <typename Signature>
		class WriteTask;

		template <typename Writable, typename ... Args>
		class WriteTask<void(Writable &, Args...)> : public Task
		{
		public:

			typedef std::function<void(Writable &, Args...)> Delegate;

		public:

			WriteTask(const Writable & writable, Delegate fn, Args ... args);
			virtual ~WriteTask() override = default;

			virtual void execute() const override;

		private:

			Delegate m_fn;
			std::tuple<Writable &, Args...> m_args;
		};

	public:

		static bool bootstrap(const SystemFactory & factory, const char * configFileName);

		static const Engine & getInstance();

	public:

		void switchScene(const char * filename);
		Scene & getCurrentScene();
		const Scene & getCurrentScene() const;
		void stop();

		template <typename SystemType>
		const std::unique_ptr<SystemType> & getSystem();
		template <typename SystemType>
		const std::unique_ptr<const SystemType> & getSystem() const;

		void addTask(std::unique_ptr<Task> && task) thread_const;

		template <typename Writable, typename Delegate, typename ... Args>
		void addWriteTask(const Writable & writable, Delegate fn, Args... args) thread_const;

	private:

		Engine(uint32 workerCount);

		void begin();
		void runFrame(float frameTime);

		Engine(const Engine &) = delete;
		Engine & operator=(const Engine &) = delete;

	private:

		thread_mutable TaskScheduler m_taskScheduler;
		std::vector<std::unique_ptr<System>> m_systems;
		std::unique_ptr<Scene, void(*)(Scene *)> m_scene;
		std::string m_nextSceneFileName;
		bool m_running;
	};
}

#include "violet/Engine.inl"

#endif