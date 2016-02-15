// ============================================================================

#include "violet/template/tuple_utilities.h"

#include <algorithm>

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

// ============================================================================
