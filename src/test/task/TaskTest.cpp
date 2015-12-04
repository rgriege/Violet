// ============================================================================

#include "test/task/TaskTest.h"

#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"
#include "violet/task/TaskScheduler.h"
#include "violet/utility/FormattedString.h"

#include <numeric>
#include <tuple>

using namespace Violet;

// ============================================================================

namespace TaskTestNamespace
{
	std::string getCurrentThreadId()
	{
		std::stringstream ss;
		ss << std::this_thread::get_id();
		return ss.str();
	}
	
	StatelessTest<uint32> makeThreadedTest(const uint32 workerCount, const uint32 taskCount)
	{
		return TestFactory::makeStateless(FormattedString<64>().sprintf("%d workers, %d tasks", workerCount, taskCount), taskCount, [=]() -> uint32
			{
				std::vector<uint32> counts(workerCount);
				TaskScheduler scheduler(workerCount);
				for (uint32 i = 0; i < taskCount; ++i)
				{
					const uint32 threadIndex = i % workerCount;
					scheduler.addTask(std::make_unique<DelegateTask>([&counts, threadIndex](){ ++counts[threadIndex]; }), threadIndex);
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
				TaskScheduler scheduler(0);
				scheduler.finishCurrentTasks();
				return true;
			}),
		TestFactory::makeStateless("0 workers 1 task", getCurrentThreadId(), []()
			{
				TaskScheduler scheduler(0);
				std::string threadId;
				scheduler.addTask(std::make_unique<DelegateTask>([&](){ threadId = getCurrentThreadId(); }));
				scheduler.finishCurrentTasks();
				return threadId;
			}),
		TestFactory::makeStateless("1 worker 0 tasks", true, []()
			{
				TaskScheduler scheduler(1);
				scheduler.finishCurrentTasks();
				return true;
			}),
		TestFactory::makeStateless("1 worker 1 task", getCurrentThreadId(), []()
			{
				TaskScheduler scheduler(1);
				std::string threadId;
				scheduler.addTask(std::make_unique<DelegateTask>([&](){ threadId = getCurrentThreadId(); }));
				scheduler.finishCurrentTasks();
				return threadId;
			}, true),
		makeThreadedTest(2, 20000),
		makeThreadedTest(4, 40000),
		TestFactory::makeStateless("dependencies", true, []()
			{
				TaskScheduler scheduler(2);
				std::atomic_uint concurrentTaskCount;
				concurrentTaskCount = 0;
				std::atomic_uint taskCount;
				taskCount = 100;
				bool collided = false;
				for (uint32 i = 0, originalTaskCount = taskCount; i < originalTaskCount; ++i)
				{
					scheduler.addTask(std::make_unique<DelegateTask>(
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
