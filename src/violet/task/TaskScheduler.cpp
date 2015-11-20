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

void TaskScheduler::executeTasks(TaskScheduler & taskScheduler, const uint32 index)
{
#ifdef DEBUG_THREAD
	try
	{
#endif
		auto task = taskScheduler.checkout(index);
		while (task != nullptr)
		{
			task->execute();
			task = taskScheduler.checkout(index);
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
	m_quit()
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
		task->execute();
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

std::unique_ptr<Task> TaskScheduler::checkout(const uint32 thread)
{
	auto & worker = m_workers[thread];

	std::unique_lock<std::mutex> queueLock(worker->m_queueMutex);
	worker->m_tasksAvailableCondition.wait(queueLock, [&]() { return !worker->m_queue.empty() || m_quit; });

	if (!worker->m_queue.empty())
	{
		std::unique_ptr<Task> task = std::move(worker->m_queue.front());
		worker->m_queue.pop();
		return task;
	}
	else
		return nullptr;
}

// ============================================================================
