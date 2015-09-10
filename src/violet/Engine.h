#ifndef VIOLET_Engine_H
#define VIOLET_Engine_H

#include "violet/Defines.h"
#include "violet/entity/Entity.h"
#include "violet/scene/Scene.h"
#include "violet/system/System.h"
#include "violet/utility/task/TaskScheduler.h"
#include "violet/utility/task/Task.h"

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

			const Writable & m_writable;
			Delegate m_fn;
			std::tuple<Args...> m_args;
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

		template <typename Writable, typename Delegate, typename ... Args>
		void addWriteTask(const Writable & writable, Delegate fn, Args... args) const;

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

#include "violet/Engine.inl"

#endif