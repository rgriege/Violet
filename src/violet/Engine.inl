// ============================================================================

#include "violet/template/TupleUtilities.h"

// ============================================================================

template <typename Writable, typename ... Args>
Violet::Engine::WriteTask<void(Writable &, Args...)>::WriteTask(const Writable & writable, Delegate fn, Args ... args) :
	Violet::Task(),
	m_fn(fn),
	m_args(const_cast<Writable &>(writable), args...)
{
}

// ----------------------------------------------------------------------------

template <typename Writable, typename ... Args>
void Violet::Engine::WriteTask<void(Writable &, Args...)>::execute() const
{
	Violet::for_all(m_fn, m_args);
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

template <typename Writable, typename Delegate, typename ... Args>
void Violet::Engine::addWriteTask(const Writable & writable, Delegate fn, Args... args) const
{
	addTask(std::make_unique<WriteTask<void(Writable &, Args...)>>(writable, fn, args...));
}

// ============================================================================
