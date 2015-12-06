// ============================================================================

template <typename ComponentType>
void Violet::Entity::addComponent(unique_val<ComponentType> && component)
{
	assert((m_componentFlags & ComponentType::getStaticFlag()) == 0);
	m_components.emplace_back(std::move(component));
	m_componentFlags |= ComponentType::getStaticFlag();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, typename ... Args>
void Violet::Entity::addComponent(Args && ... args)
{
	addComponent<ComponentType>(make_unique_val<ComponentType>(*this, std::forward<Args>(args)...));
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::Entity::hasComponent() const
{
	return (m_componentFlags & ComponentType::getStaticFlag()) != 0;
}

// ----------------------------------------------------------------------------

template <typename ... ComponentTypes>
bool Violet::Entity::hasComponents() const
{
	const uint32 componentFlags = Component::gatherFlags<ComponentTypes...>();
	return (m_componentFlags & componentFlags) == componentFlags;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::lent_ptr<ComponentType> Violet::Entity::getComponent()
{
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const unique_val<Component> & component) { return component->getFlag() == ComponentType::getStaticFlag(); });
	if (it != m_components.end())
		return lent_ptr<ComponentType>(static_cast<ComponentType *>(it->ptr()));

	return nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::lent_ptr<const ComponentType> Violet::Entity::getComponent() const
{
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const unique_val<Component> & component) { return component->getFlag() == ComponentType::getStaticFlag(); });
	if (it != m_components.end())
		return lent_ptr<const ComponentType>(static_cast<const ComponentType *>(it->ptr()));

	return nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::Entity::removeComponent()
{
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const unique_val<Component> & component) { return component->getFlag() == ComponentType::getStaticFlag(); });
	const bool found = it != m_components.end();
	if (found)
	{
		m_components.erase(it);
		m_componentFlags &= ~ComponentType::getStaticFlag();
	}
	return found;
}

// ============================================================================
