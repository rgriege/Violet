#ifndef VIOLET_Task_H
#define VIOLET_Task_H

#include "violet/Defines.h"

#include <functional>

namespace Violet
{
	class VIOLET_API Task
	{
	public:

		static const uint32 ms_defaultPriority;

	public:

		Task(uint32 priority = ms_defaultPriority);
		virtual ~Task() = default;

		virtual void execute() const = 0;

		bool operator<(const Task& rhs) const;

	private:

		const uint32 m_priority;
		const uint32 m_id;
	};

	class VIOLET_API DelegateTask : public Task
	{
	public:

		DelegateTask(std::function<void()> delegate, uint32 priority = ms_defaultPriority);
		virtual ~DelegateTask() override = default;

		virtual void execute() const override;

	private:

		const std::function<void()> m_delegate;
	};
}

#endif
