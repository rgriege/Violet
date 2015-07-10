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

// ============================================================================
