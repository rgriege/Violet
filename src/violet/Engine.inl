// ============================================================================

#include "violet/template/TupleUtilities.h"

// ============================================================================

template <typename Writable>
Violet::Engine::WriteTask<void(Writable &)>::WriteTask(const Writable & writable, Delegate fn) :
	Violet::Task(reinterpret_cast<uint64>(&writable)),
	m_fn(fn),
	m_writable(const_cast<Writable &>(writable))
{
}

// ----------------------------------------------------------------------------

template <typename Writable>
void Violet::Engine::WriteTask<void(Writable &)>::execute() const
{
	m_fn(m_writable);
}

// ============================================================================

template <typename SystemType>
const std::unique_ptr<SystemType> & Violet::Engine::getSystem()
{
	const auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const std::unique_ptr<System> & system) { return system->getLabel() == SystemType::getStaticLabel(); });
	if (it != m_systems.end())
		return (const std::unique_ptr<SystemType> &)(*it);

	static std::unique_ptr<SystemType> s_null;
	return s_null;
}

// ----------------------------------------------------------------------------

template <typename SystemType>
const std::unique_ptr<const SystemType> & Violet::Engine::getSystem() const
{
	const auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const std::unique_ptr<System> & system) { return system->getLabel() == SystemType::getStaticLabel(); });
	if (it != m_systems.end())
		return (const std::unique_ptr<const SystemType> &)(*it);

	static std::unique_ptr<const SystemType> s_null;
	return s_null;
}

// ----------------------------------------------------------------------------

template <typename Writable, typename Delegate>
void Violet::Engine::addWriteTask(const Writable & writable, Delegate fn, const Thread thread) const
{
	addTask(std::make_unique<WriteTask<void(Writable &)>>(writable, fn), thread, FrameStage::Write);
}

// ============================================================================
