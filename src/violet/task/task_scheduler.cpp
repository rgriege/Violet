// ============================================================================

#include "violet/task/task_scheduler.h"

#include "violet/log/Log.h"
#include "violet/utility/formatted_string.h"

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

using namespace vlt;

// ============================================================================

struct StopTask final : public task
{
	StopTask(std::atomic_uint & stopCount) :
		task(),
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

// ============================================================================

struct task_scheduler::worker
{
	std::queue<std::unique_ptr<task>> m_queue;
	std::mutex m_queueMutex;
	std::condition_variable m_tasksAvailableCondition;
	std::unique_ptr<std::thread> m_thread;
};

// ============================================================================

struct task_scheduler::task_guard
{
public:

	task_guard(task_scheduler & scheduler, std::unique_ptr<task> && task) :
		m_scheduler(scheduler),
		m_task(std::move(task))
	{
	}

	task_guard(task_guard && rhs) :
		m_scheduler(rhs.m_scheduler),
		m_task(std::move(rhs.m_task))
	{
	}

	task_guard & operator=(task_guard && rhs)
	{
		if (m_task != nullptr)
			m_scheduler.checkin(m_task->dependency);

		m_task = std::move(rhs.m_task);
		return *this;
	}

	~task_guard()
	{
		if (m_task != nullptr)
			m_scheduler.checkin(m_task->dependency);
	}

	const std::unique_ptr<task> & getTask() const
	{
		return m_task;
	}

private:

	task_guard(const task_guard &) = delete;
	task_guard & operator=(const task_guard &) = delete;

private:

	task_scheduler & m_scheduler;
	std::unique_ptr<task> m_task;
};

// ============================================================================

void task_scheduler::executeTasks(task_scheduler & taskScheduler, const u32 index)
{
	try
	{
		bool outOfTasks = false;
		while (!outOfTasks)
		{
			auto taskGuard = taskScheduler.checkout(index);
			if (taskGuard.getTask() != nullptr)
				taskGuard.getTask()->execute();
			else
				outOfTasks = true;
		}
	}
	catch (std::exception const & e)
	{
		log(e.what());
	}
}

// ============================================================================

task_scheduler::task_scheduler(const u32 workerCount) :
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

	for (u32 i = 0; i < workerCount; ++i)
		m_workers.emplace_back(std::make_unique<worker>());

	for (u32 i = 0; i < workerCount; ++i)
		m_workers[i]->m_thread = std::make_unique<std::thread>(std::bind(executeTasks, std::ref(*this), i));
}

// ----------------------------------------------------------------------------

task_scheduler::~task_scheduler()
{
	m_quit = true;
	for (auto & worker : m_workers)
	{
		worker->m_tasksAvailableCondition.notify_one();
		worker->m_thread->join();
	}
}

// ----------------------------------------------------------------------------

void task_scheduler::add_task(std::unique_ptr<task> && task, const int thread)
{
	assert(task != nullptr);
	if (!m_workers.empty())
		add_task(std::move(task), (thread >= 0 && thread < static_cast<int>(m_workers.size())) ? thread : (++m_nextTaskWorkerIndex % m_workers.size()), false);
	else
		m_mainThreadTaskQueue.emplace(std::move(task));
}

// ----------------------------------------------------------------------------

void task_scheduler::finishCurrentTasks()
{
	if (!m_workers.empty())
	{
		m_stopCount = 0;
		m_stopCountGoal = m_workers.size();

		for (u32 i = 0; i < m_workers.size(); ++i)
			add_task(std::make_unique<StopTask>(m_stopCount), i, true);

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

void task_scheduler::add_task(std::unique_ptr<task> && task, const u32 thread, const bool isStopTask)
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

task_scheduler::task_guard task_scheduler::checkout(const u32 thread)
{
	auto & worker = m_workers[thread];
	std::unique_ptr<task> task;

	{
		std::unique_lock<std::mutex> queueLock(worker->m_queueMutex);
		worker->m_tasksAvailableCondition.wait(queueLock, [&]() { return !worker->m_queue.empty() || m_quit; });

		if (!worker->m_queue.empty())
		{
			task = std::move(worker->m_queue.front());
			worker->m_queue.pop();
		}
	}

	if (task != nullptr && task->dependency != task::Null_Dependency)
	{
		std::unique_lock<std::mutex> busyDependenciesLock(m_busyDependenciesMutex);
		m_busyDependenciesCv.wait(busyDependenciesLock, [&]() { return m_busyDependencies.emplace(task->dependency).second; });
	}

	return task_guard(*this, std::move(task));
}

// ----------------------------------------------------------------------------

void task_scheduler::checkin(const u64 dependency)
{
	if (dependency != task::Null_Dependency)
	{
		{
			const std::lock_guard<std::mutex> guard(m_busyDependenciesMutex);
			const auto eraseCount = m_busyDependencies.erase(dependency);
			assert(eraseCount == 1);
		}
		m_busyDependenciesCv.notify_all();
	}
}

// ============================================================================
