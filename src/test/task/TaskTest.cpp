// ============================================================================

#include "test/task/tasktest.h"

#include "test/core/testevaluator.h"
#include "test/core/testfactory.h"
#include "violet/task/task_scheduler.h"
#include "violet/utility/formatted_string.h"

#include <numeric>
#include <tuple>

using namespace vlt;

// ============================================================================

namespace TaskTestNamespace
{
	std::string getCurrentThreadId()
	{
		std::stringstream ss;
		ss << std::this_thread::get_id();
		return ss.str();
	}
	
	StatelessTest<u32> makeThreadedTest(const u32 workerCount, const u32 taskCount)
	{
		return TestFactory::makeStateless(Formatted_String<64>().sprintf("%d workers, %d tasks", workerCount, taskCount), taskCount, [=]() -> u32
			{
				std::Vector<u32> counts(workerCount);
				Task_Scheduler scheduler(workerCount);
				for (u32 i = 0; i < taskCount; ++i)
				{
					const u32 threadIndex = i % workerCount;
					scheduler.add_task(std::make_unique<delegate_task>([&counts, threadIndex](){ ++counts[threadIndex]; }), threadIndex);
				}
				scheduler.finishCurrentTasks();
				return std::accumulate(counts.cbegin(), counts.cend(), 0);
			});
	}
}

using namespace TaskTestNamespace;

// ============================================================================

void TaskTest::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("task tests", std::forward_as_tuple(
		TestFactory::makeStateless("0 workers 0 tasks", true, []()
			{
				Task_Scheduler scheduler(0);
				scheduler.finishCurrentTasks();
				return true;
			}),
		TestFactory::makeStateless("0 workers 1 task", getCurrentThreadId(), []()
			{
				Task_Scheduler scheduler(0);
				std::string threadId;
				scheduler.add_task(std::make_unique<delegate_task>([&](){ threadId = getCurrentThreadId(); }));
				scheduler.finishCurrentTasks();
				return threadId;
			}),
		TestFactory::makeStateless("1 worker 0 tasks", true, []()
			{
				Task_Scheduler scheduler(1);
				scheduler.finishCurrentTasks();
				return true;
			}),
		TestFactory::makeStateless("1 worker 1 task", getCurrentThreadId(), []()
			{
				Task_Scheduler scheduler(1);
				std::string threadId;
				scheduler.add_task(std::make_unique<delegate_task>([&](){ threadId = getCurrentThreadId(); }));
				scheduler.finishCurrentTasks();
				return threadId;
			}, true),
		makeThreadedTest(2, 20000),
		makeThreadedTest(4, 40000),
		TestFactory::makeStateless("dependencies", true, []()
			{
				Task_Scheduler scheduler(2);
				std::atomic_uint concurrentTaskCount;
				concurrentTaskCount = 0;
				std::atomic_uint taskCount;
				taskCount = 100;
				bool collided = false;
				for (u32 i = 0, originalTaskCount = taskCount; i < originalTaskCount; ++i)
				{
					scheduler.add_task(std::make_unique<delegate_task>(
						[&]()
						{
							if (++concurrentTaskCount == 2)
								collided = false;
							--taskCount;
							std::this_thread::sleep_for(std::chrono::milliseconds(50));
							--concurrentTaskCount;
						}, 1));
				}
				scheduler.finishCurrentTasks();
				return !collided && taskCount == 0;
			})
		)).evaluate(evaluator);
}

// ============================================================================
