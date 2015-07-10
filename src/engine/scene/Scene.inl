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
	const auto result = s_tags.insert(ComponentType::getTag());
	if (!result.second)
	{
		std::cout << "Tag ";
		std::cout.write(result.first->asString(), 4) << " already used for a component" << std::endl;
	}
#endif
	ms_componentFactory.assign(std::string(ComponentType::getTag().asString(), 4), &Scene::factoryCreateComponent<ComponentType>);
}

// ============================================================================

template <typename ComponentType>
void Violet::Scene::factoryCreateComponent(Scene & scene, Entity & entity, Deserializer & deserializer)
{
	entity.addComponent<ComponentType>(deserializer);
}

// ============================================================================
