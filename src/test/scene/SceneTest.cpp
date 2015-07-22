#include "test/scene/SceneTest.h"

#include "engine/entity/Entity.h"
#include "engine/scene/Scene.h"
#include "engine/transform/component/TransformComponent.h"
#include "engine/update/component/UpdateComponent.h"
#include "test/core/TestEvaluator.h"
#include "test/core/TestFactory.h"

#include <tuple>

using namespace Violet;

namespace SceneTestNamespace
{
	class SceneBuilder
	{
	public:

		SceneBuilder();

		template <typename... ComponentTypes>
		SceneBuilder & addEntity();

		Scene create();

	private:

		template <typename... ComponentTypes>
		struct AddHelper;

		template <typename ComponentType, typename... ComponentTypes>
		struct AddHelper<ComponentType, ComponentTypes...>
		{
			static void add(Scene & scene, Entity & entity);
		};

		template <>
		struct AddHelper<>
		{
			static void add(Scene & scene, Entity & entity);
		};

	private:
		
		void reset();

	private:

		std::unique_ptr<Scene> m_scene;
	};

	lent_ptr<Entity> tryGet(Scene & scene, uint32 index);
}

using namespace SceneTestNamespace;

void SceneTest::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("scene tests", std::forward_as_tuple(
		TestFactory::makeStatefulSuite("empty", SceneBuilder().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("entity check", true, [](Scene & scene) { return scene.getRoot().getChildren().empty(); })
		)),
		TestFactory::makeStatefulSuite("single entity w/ 1 component", SceneBuilder().addEntity<TransformComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entity", 1u, [](Scene & scene) { return scene.getRoot().getChildren().size(); }),
			TestFactory::makeStateful<Scene>("has component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return entity != nullptr ? entity->hasComponent<TransformComponent>() : false; }),
			TestFactory::makeStateful<Scene>("access component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); auto tc = entity != nullptr ? entity->getComponent<TransformComponent>() : nullptr; return tc != nullptr; })
		)),
		TestFactory::makeStatefulSuite("single entity w/ 2 components", SceneBuilder().addEntity<TransformComponent, UpdateComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entity", 1u, [](Scene & scene) { return scene.getRoot().getChildren().size(); }),
			TestFactory::makeStateful<Scene>("has 1st component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return entity->hasComponent<TransformComponent>(); }),
			TestFactory::makeStateful<Scene>("has 2nd component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return entity->hasComponent<UpdateComponent>(); })
		)),
		TestFactory::makeStatefulSuite("2 entities w/ different components", SceneBuilder().addEntity<TransformComponent>().addEntity<UpdateComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entities", 2u, [](Scene & scene) { return scene.getRoot().getChildren().size(); }),
			TestFactory::makeStateful<Scene>("1st entity has component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return entity->hasComponent<TransformComponent>(); }),
			TestFactory::makeStateful<Scene>("2nd entity has component", true, [](Scene & scene) { auto entity = tryGet(scene, 1); return entity->hasComponent<UpdateComponent>(); })
		))
	)).evaluate(evaluator);
}

SceneTestNamespace::SceneBuilder::SceneBuilder() :
	m_scene()
{
	reset();
}

template <typename... ComponentTypes>
SceneTestNamespace::SceneBuilder & SceneTestNamespace::SceneBuilder::addEntity()
{
	Entity & entity = m_scene->getRoot().addChild();
	AddHelper<ComponentTypes...>::add(*m_scene, entity);
	return *this;
}

Scene SceneTestNamespace::SceneBuilder::create()
{
	Scene result = std::move(*m_scene.release());
	reset();
	return result;
}

template <typename ComponentType, typename... ComponentTypes>
void SceneTestNamespace::SceneBuilder::AddHelper<ComponentType, ComponentTypes...>::add(Scene & scene, Entity & entity)
{
	entity.addComponent<ComponentType>();
	AddHelper<ComponentTypes...>::add(scene, entity);
}

void SceneTestNamespace::SceneBuilder::AddHelper<>::add(Scene & /*scene*/, Entity & /*entity*/)
{
}

void SceneTestNamespace::SceneBuilder::reset()
{
	m_scene = std::make_unique<Scene>();
}

lent_ptr<Entity> SceneTestNamespace::tryGet(Scene & scene, const uint32 index)
{
	auto const & entities = scene.getRoot().getChildren();
	return index < entities.size() ? entities[index].ptr() : nullptr;
}