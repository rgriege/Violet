// ============================================================================

#ifdef _DEBUG
#include <set>
#endif

// ============================================================================

template <typename ComponentType>
void Violet::Entity::installComponent()
{
#ifdef _DEBUG
	std::set<Tag> s_tags;
	const auto result = s_tags.insert(ComponentType::getStaticTag());
	if (!result.second)
	{
		std::cout << "Tag ";
		std::cout.write(result.first->asString(), 4) << " already used for a component" << std::endl;
	}
#endif
	ms_componentFactory.assign(std::string(ComponentType::getStaticTag().asString(), 4), &Entity::factoryCreateComponent<ComponentType>);
}

// ============================================================================

template <typename ComponentType>
void Violet::Entity::addComponent(unique_val<ComponentType> && component)
{
	m_components.emplace_back(std::move(component));
	m_componentFlags |= ComponentType::getFlag();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, typename ... Args>
void Violet::Entity::addComponent(Args && ... args)
{
	m_components.emplace_back(make_unique_val<ComponentType>(*this, std::forward<Args>(args)...));
	m_componentFlags |= ComponentType::getFlag();
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::Entity::hasComponent() const
{
	return (m_componentFlags & ComponentType::getFlag()) != 0;
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
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const unique_val<Component> & component) { return component->getTag() == ComponentType::getStaticTag(); });
	if (it != m_components.end())
		return lent_ptr<ComponentType>(static_cast<ComponentType *>(it->ptr()));

	return nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
Violet::lent_ptr<const ComponentType> Violet::Entity::getComponent() const
{
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const unique_val<Component> & component) { return component->getTag() == ComponentType::getStaticTag(); });
	if (it != m_components.end())
		return lent_ptr<const ComponentType>(static_cast<const ComponentType *>(it->ptr()));

	return nullptr;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::Entity::removeComponent()
{
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const unique_val<Component> & component) { return component->getTag() == ComponentType::getStaticTag(); });
	const bool found = it != m_components.end();
	if (found)
	{
		m_components.erase(it);
		m_componentFlags &= ~ComponentType::getFlag();
	}
	return found;
}

// ============================================================================

template <typename ComponentType>
void Violet::Entity::factoryCreateComponent(Entity & entity, Deserializer & deserializer)
{
	entity.addComponent<ComponentType>(deserializer);
}

// ============================================================================
