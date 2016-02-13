#ifndef VIOLET_ENGINE_H
#define VIOLET_ENGINE_H

#include "violet/core/defines.h"
#include "violet/event/event_context_owner.h"
#include "violet/system/system.h"
#include "violet/task/task.h"
#include "violet/task/thread.h"

#include <array>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

namespace vlt
{
	struct scene;
	struct system_factory;
	struct task_scheduler;

	struct VIOLET_API engine : public event_context_owner
	{
	private:

		template <typename Signature>
		struct WriteTask;

		template <typename Writable>
		struct WriteTask<void(Writable &)> : public task
		{
		public:

			typedef std::function<void(Writable &)> delegate;

		public:

			WriteTask(const Writable & writable, delegate fn);
			virtual ~WriteTask() override = default;

			virtual void execute() const override;

		private:

			delegate m_fn;
			Writable & m_writable;
		};

		enum struct FrameStage
		{
			Read,
			Write,
			Delete,
			Last = Delete,
			Count = Last + 1
		};

		struct TaskQueue
		{
			std::queue<std::pair<std::unique_ptr<task>, thread>> m_tasks;
			std::mutex m_mutex;
		};

	public:

		static bool bootstrap(const system_factory & factory, const char * configFileName);

		static const engine & instance();

	public:

		~engine();

		void switch_scene(const char * filename);
		scene & get_current_scene();
		const scene & get_current_scene() const;
		void stop();

		void add_system(std::unique_ptr<system> && s);
		template <typename SystemType>
		const std::unique_ptr<SystemType> & get_system();
		template <typename SystemType>
		const std::unique_ptr<const SystemType> & get_system() const;

		void add_read_task(std::unique_ptr<task> && task, thread thread = thread::Any) thread_const;
		template <typename Writable, typename delegate>
		void add_write_task(const Writable & writable, delegate fn, thread thread = thread::Any) thread_const;
		void add_delete_task(std::unique_ptr<task> && task, thread thread = thread::Any) thread_const;

	private:

		engine(u32 workerCount);

		void begin();
		void run_frame(r32 frameTime);
		void perform_current_frame_stage();

		void add_task(std::unique_ptr<task> && task, thread thread, FrameStage frameStage) thread_const;
		bool has_tasks() const;

		engine(const engine &) = delete;
		engine & operator=(const engine &) = delete;

	private:

		thread_mutable std::unique_ptr<task_scheduler> m_taskScheduler;
		thread_mutable std::array<TaskQueue, static_cast<int>(FrameStage::Count)> m_taskQueues;
		std::vector<std::unique_ptr<system>> m_systems;
		std::unique_ptr<scene> m_scene;
		std::string m_nextSceneFileName;
		bool m_running;
		FrameStage m_frameStage;
	};
}

#include "violet/core/engine.inl"

#endif
