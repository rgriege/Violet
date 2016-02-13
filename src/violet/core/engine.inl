// ============================================================================

#include "violet/template/tuple_utilities.h"

#include <algorithm>

// ============================================================================

template <typename Writable>
vlt::engine::WriteTask<void(Writable &)>::WriteTask(const Writable & writable, delegate fn) :
	vlt::task(reinterpret_cast<u64>(&writable)),
	m_fn(fn),
	m_writable(const_cast<Writable &>(writable))
{
}

// ----------------------------------------------------------------------------

template <typename Writable>
void vlt::engine::WriteTask<void(Writable &)>::execute() const
{
	m_fn(m_writable);
}

// ============================================================================

template <typename SystemType>
const std::unique_ptr<SystemType> & vlt::engine::get_system()
{
	const auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const std::unique_ptr<system> & system) { return system->label == SystemType::get_label_static(); });
	if (it != m_systems.end())
		return (const std::unique_ptr<SystemType> &)(*it);

	static std::unique_ptr<SystemType> s_null;
	return s_null;
}

// ----------------------------------------------------------------------------

template <typename SystemType>
const std::unique_ptr<const SystemType> & vlt::engine::get_system() const
{
	const auto it = std::find_if(m_systems.begin(), m_systems.end(), [](const std::unique_ptr<system> & system) { return system->label == SystemType::get_label_static(); });
	if (it != m_systems.end())
		return (const std::unique_ptr<const SystemType> &)(*it);

	static std::unique_ptr<const SystemType> s_null;
	return s_null;
}

// ----------------------------------------------------------------------------

template <typename Writable, typename delegate>
void vlt::engine::add_write_task(const Writable & writable, delegate fn, const thread thread) const
{
	add_task(std::make_unique<WriteTask<void(Writable &)>>(writable, fn), thread, FrameStage::Write);
}

// ============================================================================
