// ============================================================================

#ifdef _DEBUG
#include <set>
#endif

// ============================================================================

template <typename ComponentType>
void Violet::Scene::installComponent()
{
#ifdef _DEBUG
	std::set<Tag> s_tags;
	const auto result = s_tags.insert(ComponentType::getTypeId());
	if (!result.second)
	{
		std::cout << "Tag ";
		std::cout.write(result.first->asString(), 4) << " already used for a component" << std::endl;
	}
#endif
	ms_componentFactory.assign(std::string(ComponentType::getTypeId().asString(), 4), &Scene::factoryCreateComponent<ComponentType>);
}

// ============================================================================

template <typename ComponentType, typename... Args>
ComponentType & Violet::Scene::createComponent(const Entity entity, Args&&... args)
{
	return m_componentManager.create<ComponentType>(entity, std::forward<Args>(args)...);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::Scene::hasComponent(const Entity entity) const
{
	return m_componentManager.has<ComponentType>(entity);
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
ComponentType * Violet::Scene::getComponent(const Entity entity)
{
	return m_componentManager.get<ComponentType>(entity);
}

// ----------------------------------------------------------------------------

template <typename... ComponentTypes>
Violet::ComponentManager::View<ComponentTypes...> Violet::Scene::getView()
{
	return m_componentManager.getView<ComponentTypes...>();
}

// ----------------------------------------------------------------------------

template <typename ComponentType>
bool Violet::Scene::removeComponent(const Entity entity)
{
	return m_componentManager.remove<ComponentType>(entity);
}

// ============================================================================

template <typename ComponentType>
void Violet::Scene::factoryCreateComponent(Scene & scene, const Entity entity, Deserializer & deserializer)
{
	scene.createComponent<ComponentType>(entity, deserializer);
}

// ============================================================================
