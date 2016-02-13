#ifndef VIOLET_TASK_H
#define VIOLET_TASK_H

#include "violet/core/defines.h"

#include <functional>

namespace vlt
{
	struct VIOLET_API task
	{
		const u64 dependency;

		static const u64 Null_Dependency;

	public:

		task(u64 dependency = Null_Dependency);
		virtual ~task() = default;

		virtual void execute() const = 0;
	};

	struct VIOLET_API delegate_task : public task
	{
		delegate_task(std::function<void()> delegate, u64 dependency = Null_Dependency);
		virtual ~delegate_task() override = default;

		virtual void execute() const override;

	private:

		const std::function<void()> m_delegate;
	};
}

#endif
