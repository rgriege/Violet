// ============================================================================

template <typename SystemType>
SystemType * Violet::Engine::fetch()
{
	auto it = std::find_if(m_systems.begin(), m_systems.end(), [](std::unique_ptr<System> & system) { return system->getLabel() == SystemType::getStaticLabel(); });
	return it != m_systems.end() ? static_cast<SystemType*>(it->get()) : nullptr;
}

// ============================================================================
