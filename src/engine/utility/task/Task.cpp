// ============================================================================

#include "engine/utility/task/Task.h"

using namespace Violet;

// ============================================================================

unsigned n = 0;

const uint32 Task::ms_defaultPriority = 32;

// ============================================================================

Task::Task(const uint32 priority) :
	m_priority(priority),
	m_id(++n)
{
}

// ----------------------------------------------------------------------------

bool Task::operator<(const Task & rhs) const
{
	return m_priority < rhs.m_priority;
}

// ============================================================================

DelegateTask::DelegateTask(std::function<void()> delegate, const uint32 priority) :
	Task(priority),
	m_delegate(delegate)
{
}

// ----------------------------------------------------------------------------

void DelegateTask::execute() const
{
	m_delegate();
}

// ============================================================================