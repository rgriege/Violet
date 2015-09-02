// ============================================================================

#include "violet/utility/task/TaskScheduler.h"

#include <assert.h>

#if 0
#include <iostream>
#define LOG(message) std::cout << std::this_thread::get_id() << " " << message << std::endl
#else
#define LOG(message) (void(0))
#endif

using namespace Violet;

// ============================================================================

void TaskScheduler::executeTasks(TaskScheduler & taskScheduler)
{
	std::unique_ptr<Task> task = taskScheduler.checkOut();
	while (task != nullptr)
	{
		task->execute();
		task = taskScheduler.checkOut();
	}
}

// ============================================================================

TaskScheduler::TaskScheduler(const uint32 workerCount) :
	m_threads(),
	m_queue(),
	m_queueMutex(),
	m_tasksAvailableCondition(),
	m_tasksCompleteCondition(),
	m_idleThreadCount(0)
{
	for (uint32 i = 0; i < workerCount; ++i)
		m_threads.emplace_back(std::bind(executeTasks, std::ref(*this)));
}

// ----------------------------------------------------------------------------

TaskScheduler::~TaskScheduler()
{
	for (auto & thread : m_threads)
		thread.join();
}

// ----------------------------------------------------------------------------

void TaskScheduler::addTask(std::unique_ptr<Task> && task)
{
	assert(task != NULL);
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_queue.push(std::move(task));
	}
	m_tasksAvailableCondition.notify_one();
}

// ----------------------------------------------------------------------------

void TaskScheduler::clear()
{
	std::queue<std::unique_ptr<Task>> emptyQueue;
	std::unique_lock<std::mutex> lock(m_queueMutex);
	m_queue.swap(emptyQueue);
}

// ----------------------------------------------------------------------------

void TaskScheduler::finishCurrentTasks()
{
	if (m_threads.size() == 0)
	{
		while (!m_queue.empty())
		{
			m_queue.front()->execute();
			m_queue.pop();
		}
	}
	else
	{
		std::unique_lock<std::mutex> lock(m_idleThreadCountMutex);
		LOG("Waiting on workers...");
		m_tasksCompleteCondition.wait(lock, [&]() { return m_idleThreadCount == m_threads.size(); });
	}
}

// ============================================================================

std::unique_ptr<Task> TaskScheduler::checkOut()
{
	std::unique_lock<std::mutex> queueLock(m_queueMutex);
	while (m_queue.empty())
	{
		m_idleThreadCountMutex.lock();
		const uint32 idleThreadCount = ++m_idleThreadCount;
		LOG("Idling...");
		m_idleThreadCountMutex.unlock();

		if (idleThreadCount == m_threads.size())
			m_tasksCompleteCondition.notify_one();

		m_tasksAvailableCondition.wait(queueLock);

		m_idleThreadCountMutex.lock();
		LOG("Unidling");
		--m_idleThreadCount;
		m_idleThreadCountMutex.unlock();
	}

	std::unique_ptr<Task> task = std::move(m_queue.front());
	m_queue.pop();
    return task;
}

// ============================================================================
