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
	const auto result = s_tags.insert(ComponentType::getTag());
	if (!result.second)
	{
		std::cout << "Tag ";
		std::cout.write(result.first->asString(), 4) << " already used for a component" << std::endl;
	}
#endif
	ms_componentFactory.assign(std::string(ComponentType::getTag().asString(), 4), &Entity::factoryCreateComponent<ComponentType>);
}

// ============================================================================

template <typename ComponentType>
void Violet::Entity::addComponent(std::unique_ptr<ComponentType> && component)
{
	m_components.emplace_back(std::move(component));
	m_componentFlags |= ComponentType::getFlag();
}

// ----------------------------------------------------------------------------

template <typename ComponentType, typename ... Args>
void Violet::Entity::addComponent(Args && ... args)
{
	m_components.emplace_back(std::make_unique<ComponentType>(*this, std::forward<Args>(args)...));
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
const std::unique_ptr<ComponentType> & Violet::Entity::getComponent()
{
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const std::unique_ptr<Component> & component) { return component->getTag() == ComponentType::getTag(); });
	if (it != m_components.end())
		return (const std::unique_ptr<ComponentType> &)(*it);

	static std::unique_ptr<ComponentType> s_null;
	return s_null;
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
const std::unique_ptr<const ComponentType> & Violet::Entity::getComponent() const
{
	const auto it = std::find_if(m_components.begin(), m_components.end(), [](const std::unique_ptr<Component> & component) { return component->getTag() == ComponentType::getTag(); });
	if (it != m_components.end())
		return (const std::unique_ptr<const ComponentType> &)(*it);

	static std::unique_ptr<const ComponentType> s_null;
	return s_null;
}

// ============================================================================

template <typename ComponentType>
void Violet::Entity::factoryCreateComponent(Entity & entity, Deserializer & deserializer)
{
	entity.addComponent<ComponentType>(deserializer);
}

// ============================================================================
