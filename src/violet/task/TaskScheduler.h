#ifndef VIOLET_TaskScheduler_H
#define VIOLET_TaskScheduler_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>

#include "violet/Defines.h"
#include "violet/task/Task.h"

namespace Violet
{
	class VIOLET_API TaskScheduler final
	{
	private:

		class TaskGuard;
		struct Worker;

	private:

		static void executeTasks(TaskScheduler & taskScheduler, uint32 index);

	public:

		TaskScheduler(uint32 workerCount);
		~TaskScheduler();

		void addTask(std::unique_ptr<Task> && task, int thread = -1);
		void finishCurrentTasks();

	private:

		void addTask(std::unique_ptr<Task> && task, uint32 thread, bool isStopTask);

		TaskGuard checkout(uint32 thread);
		void checkin(uint64 dependency);

		TaskScheduler(const TaskScheduler &) = delete;
		TaskScheduler & operator=(const TaskScheduler &) = delete;

	private:

		std::vector<std::unique_ptr<Worker>> m_workers;
		std::atomic_uint m_nextTaskWorkerIndex;
		std::atomic_uint m_stopCount;
		std::atomic_uint m_stopCountGoal;
		std::atomic_bool m_quit;
		std::queue<std::unique_ptr<Task>> m_mainThreadTaskQueue;
		std::set<uint64> m_busyDependencies;
		std::mutex m_busyDependenciesMutex;
		std::condition_variable m_busyDependenciesCv;
	};
}

#endif
