// ============================================================================

#include <algorithm>

// ============================================================================

template <typename SystemType>
const std::unique_ptr<SystemType> & vlt::Engine::get_system()
{
	const auto it = std::find_if(systems.begin(), systems.end(), [](const std::unique_ptr<System> & system) { return system->label == SystemType::get_label_static(); });
	if (it != systems.end())
		return (const std::unique_ptr<SystemType> &)(*it);

	static std::unique_ptr<SystemType> s_null;
	return s_null;
}

// ----------------------------------------------------------------------------

template <typename SystemType>
const std::unique_ptr<const SystemType> & vlt::Engine::get_system() const
{
	const auto it = std::find_if(systems.begin(), systems.end(), [](const std::unique_ptr<System> & system) { return system->label == SystemType::get_label_static(); });
	if (it != systems.end())
		return (const std::unique_ptr<const SystemType> &)(*it);

	static std::unique_ptr<const SystemType> s_null;
	return s_null;
}

// ============================================================================
