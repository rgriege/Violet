#ifndef VIOLET_TaskScheduler_H
#define VIOLET_TaskScheduler_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "violet/Defines.h"
#include "violet/task/Task.h"

namespace Violet
{
	class TaskScheduler final
	{
	private:

		static void executeTasks(TaskScheduler & taskScheduler);

	public:

		TaskScheduler(uint32 workerCount);
		~TaskScheduler();

		void addTask(std::unique_ptr<Task> && task);
		void clear();
		void finishCurrentTasks();

	private:

		std::unique_ptr<Task> checkOut();

		TaskScheduler(const TaskScheduler &) = delete;
		TaskScheduler & operator=(const TaskScheduler &) = delete;

	private:

		std::vector<std::thread> m_threads;
		std::queue<std::unique_ptr<Task>> m_queue;
		std::mutex m_queueMutex;
		std::mutex m_idleThreadCountMutex;
		std::condition_variable m_tasksAvailableCondition;
		std::condition_variable m_tasksCompleteCondition;
		uint32 m_idleThreadCount;
	};
}

#endif
