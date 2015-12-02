// ============================================================================

#include "violet/task/Task.h"

using namespace Violet;

// ============================================================================

const uint64 Task::ms_nullDependency = 0;
const uint32 Task::ms_defaultPriority = 32;

// ============================================================================

Task::Task(const uint64 dependency, const uint32 priority) :
	m_dependency(dependency),
	m_priority(priority)
{
}

// ----------------------------------------------------------------------------

uint64 Task::getDependency() const
{
	return m_dependency;
}

// ----------------------------------------------------------------------------

bool Task::operator<(const Task & rhs) const
{
	return m_priority < rhs.m_priority;
}

// ============================================================================

DelegateTask::DelegateTask(std::function<void()> delegate, const uint64 dependency, const uint32 priority) :
	Task(dependency, priority),
	m_delegate(delegate)
{
}

// ----------------------------------------------------------------------------

void DelegateTask::execute() const
{
	m_delegate();
}

// ============================================================================