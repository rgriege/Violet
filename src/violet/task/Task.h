#ifndef VIOLET_Task_H
#define VIOLET_Task_H

#include "violet/Defines.h"

#include <functional>

namespace Violet
{
	class VIOLET_API Task
	{
	public:

		static const uint64 ms_nullDependency;
		static const uint32 ms_defaultPriority;

	public:

		Task(uint64 dependency = ms_nullDependency, uint32 priority = ms_defaultPriority);
		virtual ~Task() = default;

		virtual void execute() const = 0;

		uint64 getDependency() const;
		bool operator<(const Task& rhs) const;

	private:

		const uint64 m_dependency;
		const uint32 m_priority;
	};

	class VIOLET_API DelegateTask : public Task
	{
	public:

		DelegateTask(std::function<void()> delegate, uint64 dependency = ms_nullDependency, uint32 priority = ms_defaultPriority);
		virtual ~DelegateTask() override = default;

		virtual void execute() const override;

	private:

		const std::function<void()> m_delegate;
	};
}

#endif
