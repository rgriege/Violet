// ============================================================================

#include "violet/task/task.h"

using namespace vlt;

// ============================================================================

const u64 task::Null_Dependency = 0;

// ============================================================================

task::task(const u64 dependency) :
	dependency(dependency)
{
}

// ============================================================================

delegate_task::delegate_task(std::function<void()> delegate, const u64 dependency) :
	task(dependency),
	m_delegate(delegate)
{
}

// ----------------------------------------------------------------------------

void delegate_task::execute() const
{
	m_delegate();
}

// ============================================================================