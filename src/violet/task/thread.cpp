// ============================================================================

#include <assert.h>
#include <array>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "violet/log/log.h"
#include "violet/structures/vector.h"
#include "violet/task/thread.h"

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

struct Task
{
	void(*fn)(void*);
	void * mem;
};

// ============================================================================

struct Worker
{
	std::queue<Task> queue;
	std::mutex queue_mtx;
	std::condition_variable tasks_available_cnd;
	std::unique_ptr<std::thread> thread;
};

// ============================================================================

struct Deferred_Task
{
	Task task;
	u32 thread;
};

struct Deferred_Task_Queue
{
	std::queue<Deferred_Task> queue;
	std::mutex mtx;
};

// ============================================================================

task_type current_stage;
std::array<Deferred_Task_Queue, task_type::cnt> stage_task_queues;
std::vector<std::unique_ptr<Worker>> workers;
std::queue<Task> workerless_queue;
std::atomic_uint stop_cnt;
std::atomic_uint stop_cnt_goal;
std::atomic_bool quit;

// ============================================================================

void stop(void * mem)
{
	assert(!mem);
	++stop_cnt;
}

// ----------------------------------------------------------------------------

void execute_tasks(const u32 thread_index)
{
	assert(thread_index < workers.size());
	try
	{
		auto & worker = *workers[thread_index];
		while (true)
		{
			Task t{ nullptr, nullptr };

			{
				std::unique_lock<std::mutex> queueLock(worker.queue_mtx);
				while (worker.queue.empty() && !quit)
					worker.tasks_available_cnd.wait(queueLock);

				if (!worker.queue.empty())
				{
					t = worker.queue.front();
					worker.queue.pop();
				}
			}

			if (t.fn != nullptr)
				t.fn(t.mem);
			else
				break;
		}
	}
	catch (std::exception const & e)
	{
		log(e.what());
	}
}

// ============================================================================

void vlt::init_thread_pool(const u32 cnt)
{
	current_stage = read;
	stop_cnt = 0;
	stop_cnt_goal = 0;
	quit = false;

	for (u32 i = 0; i < cnt; ++i)
		workers.emplace_back(std::make_unique<Worker>());

	for (u32 i = 0; i < cnt; ++i)
		workers[i]->thread = std::make_unique<std::thread>(std::bind(execute_tasks, i));
}

// ----------------------------------------------------------------------------

void vlt::cleanup_thread_pool()
{
	finish_all_tasks();

	quit = true;
	for (auto & worker : workers)
	{
		worker->tasks_available_cnd.notify_one();
		worker->thread->join();
	}
	workers.clear();
}

// ----------------------------------------------------------------------------

void vlt::add_task(task_fn fn, void * memory, const u32 thread, task_type type)
{
	if (!workers.empty())
	{
		if (type == current_stage)
		{
			assert(thread < workers.size());
			auto & worker = *workers[thread];
			{
				const std::lock_guard<std::mutex> guard(worker.queue_mtx);
				worker.queue.emplace(Task{ fn, memory });
				if (fn != stop && stop_cnt_goal != 0)
				{
					++stop_cnt_goal;
					worker.queue.emplace(Task{ stop, nullptr });
				}
			}
			worker.tasks_available_cnd.notify_one();
		}
		else
		{
			auto & deferred_queue = stage_task_queues[type];
			const std::lock_guard<std::mutex> guard(deferred_queue.mtx);
			deferred_queue.queue.emplace(Deferred_Task{ Task{ fn, memory }, thread });
		}
	}
	else
	{
		workerless_queue.emplace(Task{ fn, memory });
	}
}

// ----------------------------------------------------------------------------

task_type vlt::get_current_stage()
{
	return current_stage;
}

// ----------------------------------------------------------------------------

void finish_current_tasks()
{
	if (!workers.empty())
	{
		stop_cnt = 0;
		stop_cnt_goal = workers.size();

		for (u32 i = 0, end = stop_cnt_goal; i < end; ++i)
			add_task(stop, nullptr, i, current_stage);

		while (stop_cnt.load() != stop_cnt_goal.load())
			;

		stop_cnt_goal = 0;
	}
	else
	{
		while (!workerless_queue.empty())
		{
			auto task = workerless_queue.front();
			task.fn(task.mem);
			workerless_queue.pop();
		}
	}
}

// ----------------------------------------------------------------------------

void vlt::complete_frame_stage()
{
	Deferred_Task_Queue & deferred_queue = stage_task_queues[current_stage];
	{
		const std::lock_guard<std::mutex> lk(deferred_queue.mtx);
		while (!deferred_queue.queue.empty())
		{
			const Deferred_Task & deferred_task = deferred_queue.queue.front();
			add_task(deferred_task.task.fn, deferred_task.task.mem, deferred_task.thread, current_stage);
			deferred_queue.queue.pop();
		}
	}

	finish_current_tasks();

	// no threads should be working at this point
	current_stage = static_cast<task_type>((static_cast<int>(current_stage) + 1) % static_cast<int>(task_type::cnt));
}

// ----------------------------------------------------------------------------

bool has_tasks()
{
	for (auto const & deferred_queue : stage_task_queues)
		if (!deferred_queue.queue.empty())
			return true;
	return false;
}

// ----------------------------------------------------------------------------

void vlt::finish_all_tasks()
{
	finish_current_tasks();

	while (has_tasks())
		complete_frame_stage();	
}

// ============================================================================
