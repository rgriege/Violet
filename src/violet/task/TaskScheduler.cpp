// ============================================================================

#include "violet/task/TaskScheduler.h"

#include "violet/utility/FormattedString.h"

#include <assert.h>

//#define DEBUG_THREAD

#ifdef DEBUG_THREAD
#include <iostream>
std::mutex ms_logMutex;
#define LOG(message) \
	 ms_logMutex.lock(); \
	std::cout << std::this_thread::get_id() << " " << message << std::endl << std::flush; \
	ms_logMutex.unlock();
#else
#define LOG(message) (void(0)) 
#endif

using namespace Violet;

// ============================================================================

namespace TaskSchedulerNamespace
{
	class StopTask final : public Task
	{
	public:

		StopTask(std::atomic_uint & stopCount) :
			Task(),
			m_stopCount(stopCount)
		{
		}

		virtual void execute() const override
		{
			++m_stopCount;
		}

	private:

		std::atomic_uint & m_stopCount;
	};
}

using namespace TaskSchedulerNamespace;

// ============================================================================

struct TaskScheduler::Worker
{
	std::queue<std::unique_ptr<Task>> m_queue;
	std::mutex m_queueMutex;
	std::condition_variable m_tasksAvailableCondition;
	std::unique_ptr<std::thread> m_thread;
};

// ============================================================================

class TaskScheduler::TaskGuard
{
public:

	TaskGuard(TaskScheduler & scheduler, std::unique_ptr<Task> && task) :
		m_scheduler(scheduler),
		m_task(std::move(task))
	{
	}

	TaskGuard(TaskGuard && rhs) :
		m_scheduler(rhs.m_scheduler),
		m_task(std::move(rhs.m_task))
	{
	}

	TaskGuard & operator=(TaskGuard && rhs)
	{
		if (m_task != nullptr)
			m_scheduler.checkin(m_task->getDependency());

		m_task = std::move(rhs.m_task);
		return *this;
	}

	~TaskGuard()
	{
		if (m_task != nullptr)
			m_scheduler.checkin(m_task->getDependency());
	}

	const std::unique_ptr<Task> & getTask() const
	{
		return m_task;
	}

private:

	TaskGuard(const TaskGuard &) = delete;
	TaskGuard & operator=(const TaskGuard &) = delete;

private:

	TaskScheduler & m_scheduler;
	std::unique_ptr<Task> m_task;
};

// ============================================================================

void TaskScheduler::executeTasks(TaskScheduler & taskScheduler, const uint32 index)
{
#ifdef DEBUG_THREAD
	try
	{
#endif
		bool outOfTasks = false;
		while (!outOfTasks)
		{
			auto taskGuard = taskScheduler.checkout(index);
			if (taskGuard.getTask() != nullptr)
				taskGuard.getTask()->execute();
			else
				outOfTasks = true;
		}
#ifdef DEBUG_THREAD
	}
	catch (std::exception const & e)
	{
		LOG(e.what());
	}
#endif
}

// ============================================================================

TaskScheduler::TaskScheduler(const uint32 workerCount) :
	m_workers(),
	m_nextTaskWorkerIndex(),
	m_stopCount(),
	m_stopCountGoal(),
	m_quit(),
	m_mainThreadTaskQueue()
{
	m_nextTaskWorkerIndex = 0;
	m_stopCount = 0;
	m_stopCountGoal = 0;
	m_quit = false;

	for (uint32 i = 0; i < workerCount; ++i)
		m_workers.emplace_back(std::make_unique<Worker>());

	for (uint32 i = 0; i < workerCount; ++i)
		m_workers[i]->m_thread = std::make_unique<std::thread>(std::bind(executeTasks, std::ref(*this), i));
}

// ----------------------------------------------------------------------------

TaskScheduler::~TaskScheduler()
{
	m_quit = true;
	for (auto & worker : m_workers)
	{
		worker->m_tasksAvailableCondition.notify_one();
		worker->m_thread->join();
	}
}

// ----------------------------------------------------------------------------

void TaskScheduler::addTask(std::unique_ptr<Task> && task, const int thread)
{
	assert(task != nullptr);
	if (!m_workers.empty())
		addTask(std::move(task), (thread >= 0 && thread < static_cast<int>(m_workers.size())) ? thread : (++m_nextTaskWorkerIndex % m_workers.size()), false);
	else
		m_mainThreadTaskQueue.emplace(std::move(task));
}

// ----------------------------------------------------------------------------

void TaskScheduler::finishCurrentTasks()
{
	if (!m_workers.empty())
	{
		m_stopCount = 0;
		m_stopCountGoal = m_workers.size();

		for (uint32 i = 0; i < m_workers.size(); ++i)
			addTask(std::make_unique<StopTask>(m_stopCount), i, true);

		while (m_stopCount.load() != m_stopCountGoal.load())
			;

		m_stopCountGoal = 0;
	}
	else
	{
		while (!m_mainThreadTaskQueue.empty())
		{
			m_mainThreadTaskQueue.front()->execute();
			m_mainThreadTaskQueue.pop();
		}
	}
}

// ============================================================================

void TaskScheduler::addTask(std::unique_ptr<Task> && task, const uint32 thread, const bool isStopTask)
{
	auto & worker = m_workers[thread];
	{
		const std::lock_guard<std::mutex> guard(worker->m_queueMutex);
		worker->m_queue.emplace(std::move(task));
		if (!isStopTask && m_stopCountGoal != 0)
		{
			++m_stopCountGoal;
			worker->m_queue.emplace(std::make_unique<StopTask>(m_stopCount));
		}
	}
	worker->m_tasksAvailableCondition.notify_one();
}

// ----------------------------------------------------------------------------

TaskScheduler::TaskGuard TaskScheduler::checkout(const uint32 thread)
{
	auto & worker = m_workers[thread];
	std::unique_ptr<Task> task;

	{
		std::unique_lock<std::mutex> queueLock(worker->m_queueMutex);
		worker->m_tasksAvailableCondition.wait(queueLock, [&]() { return !worker->m_queue.empty() || m_quit; });

		if (!worker->m_queue.empty())
		{
			task = std::move(worker->m_queue.front());
			worker->m_queue.pop();
		}
	}

	if (task != nullptr && task->getDependency() != Task::ms_nullDependency)
	{
		std::unique_lock<std::mutex> busyDependenciesLock(m_busyDependenciesMutex);
		m_busyDependenciesCv.wait(busyDependenciesLock, [&]() { return m_busyDependencies.emplace(task->getDependency()).second; });
	}

	return TaskGuard(*this, std::move(task));
}

// ----------------------------------------------------------------------------

void TaskScheduler::checkin(const uint64 dependency)
{
	if (dependency != Task::ms_nullDependency)
	{
		{
			const std::lock_guard<std::mutex> guard(m_busyDependenciesMutex);
			assert(m_busyDependencies.erase(dependency) == 1);
		}
		m_busyDependenciesCv.notify_all();
	}
}

// ============================================================================
