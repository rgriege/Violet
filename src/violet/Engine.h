#ifndef VIOLET_Engine_H
#define VIOLET_Engine_H

#include "violet/Defines.h"
#include "violet/event/EventContextOwner.h"
#include "violet/scene/SceneProcessor.h"
#include "violet/task/Task.h"

#include <array>
#include <memory>
#include <vector>

namespace Violet
{
	class Scene;
	class System;
	class SystemFactory;
	class TaskScheduler;

	class VIOLET_API Engine : public EventContextOwner
	{
	public:

		enum class Thread
		{
			Any = -1,
			Window
		};

	private:

		template <typename Signature>
		class WriteTask;

		template <typename Writable>
		class WriteTask<void(Writable &)> : public Task
		{
		public:

			typedef std::function<void(Writable &)> Delegate;

		public:

			WriteTask(const Writable & writable, Delegate fn);
			virtual ~WriteTask() override = default;

			virtual void execute() const override;

		private:

			Delegate m_fn;
			Writable & m_writable;
		};

		enum class FrameStage
		{
			Read,
			Write,
			Delete,
			Last = Delete,
			Count = Last + 1
		};

		struct TaskQueue
		{
			std::queue<std::pair<std::unique_ptr<Task>, Thread>> m_tasks;
			std::mutex m_mutex;
		};

	public:

		static bool bootstrap(const SystemFactory & factory, const char * configFileName);

		static const Engine & getInstance();

	public:

		~Engine();

		void switchScene(const char * filename);
		Scene & getCurrentScene();
		const Scene & getCurrentScene() const;
		void addSceneDelegate(SceneProcessor::Filter filter, const SceneProcessor::Delegate & delegate);
		void removeSceneDelegate(SceneProcessor::Filter filter, const SceneProcessor::Delegate & delegate);
		void stop();

		void addSystem(std::unique_ptr<System> && system);
		template <typename SystemType>
		const std::unique_ptr<SystemType> & getSystem();
		template <typename SystemType>
		const std::unique_ptr<const SystemType> & getSystem() const;

		void addReadTask(std::unique_ptr<Task> && task, Thread thread = Thread::Any) thread_const;
		template <typename Writable, typename Delegate>
		void addWriteTask(const Writable & writable, Delegate fn, Thread thread = Thread::Any) thread_const;
		void addDeleteTask(std::unique_ptr<Task> && task, Thread thread = Thread::Any) thread_const;

	private:

		Engine(uint32 workerCount);

		void begin();
		void runFrame(float frameTime);
		void performCurrentFrameStage();

		void addTask(std::unique_ptr<Task> && task, Thread thread, FrameStage frameStage) thread_const;

		Engine(const Engine &) = delete;
		Engine & operator=(const Engine &) = delete;

	private:

		thread_mutable std::unique_ptr<TaskScheduler> m_taskScheduler;
		thread_mutable std::array<TaskQueue, static_cast<int>(FrameStage::Count)> m_taskQueues;
		std::vector<std::unique_ptr<System>> m_systems;
		std::unique_ptr<Scene, void(*)(Scene *)> m_scene;
		std::unique_ptr<SceneProcessor> m_sceneProcessor;
		std::string m_nextSceneFileName;
		bool m_running;
		FrameStage m_frameStage;
	};
}

#include "violet/Engine.inl"

#endif