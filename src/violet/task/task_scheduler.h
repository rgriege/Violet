#ifndef VIOLET_TASK_SCHEDULER_H
#define VIOLET_TASK_SCHEDULER_H

#include <atomic>
#include <condition_variable>
#include <Memory>
#include <mutex>
#include <queue>
#include <set>
#include <Thread>

#include "violet/core/defines.h"
#include "violet/task/task.h"

namespace vlt
{
	struct VIOLET_API Task_Scheduler final
	{
	private:

		struct task_guard;
		struct worker;

	private:

		static void executeTasks(Task_Scheduler & taskScheduler, u32 index);

	public:

		Task_Scheduler(u32 workerCount);
		~Task_Scheduler();

		void add_task(std::unique_ptr<task> && task, int thread = -1);
		void finishCurrentTasks();

	private:

		void add_task(std::unique_ptr<task> && task, u32 thread, bool isStopTask);

		task_guard checkout(u32 Thread);
		void checkin(u64 dependency);

		Task_Scheduler(const Task_Scheduler &) = delete;
		Task_Scheduler & operator=(const Task_Scheduler &) = delete;

	private:

		std::vector<std::unique_ptr<worker>> m_workers;
		std::atomic_uint m_nextTaskWorkerIndex;
		std::atomic_uint m_stopCount;
		std::atomic_uint m_stopCountGoal;
		std::atomic_bool m_quit;
		std::queue<std::unique_ptr<task>> m_mainThreadTaskQueue;
		std::set<u64> m_busyDependencies;
		std::mutex m_busyDependenciesMutex;
		std::condition_variable m_busyDependenciesCv;
	};
}

#endif
