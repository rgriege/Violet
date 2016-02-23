#include "test/scene/scenetest.h"

#include "violet/entity/entity.h"
#include "violet/scene/scene.h"
#include "violet/transform/component/transformcomponent.h"
#include "violet/update/component/updatecomponent.h"
#include "test/core/testevaluator.h"
#include "test/core/testfactory.h"

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
			static void add(Scene & Scene, Entity & entity);
		};

	private:
		
		void reset();

	private:

		std::unique_ptr<Scene> m_scene;
	};

	template <>
	struct SceneBuilder::AddHelper<>
	{
		static void add(Scene & Scene, Entity & entity);
	};

	Lent_Ptr<Entity> tryGet(Scene & Scene, uint32 index);
}

using namespace SceneTestNamespace;

void SceneTest::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("Scene tests", std::forward_as_tuple(
		TestFactory::makeStatefulSuite("empty", SceneBuilder().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("entity check", true, [](Scene & Scene) { return Scene.getRoot().getChildren().empty(); })
		)),
		TestFactory::makeStatefulSuite("single entity w/ 1 component", SceneBuilder().addEntity<TransformComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entity", (size_t)1, [](Scene & Scene) { return Scene.getRoot().getChildren().size(); }),
			TestFactory::makeStateful<Scene>("has component", true, [](Scene & Scene) { auto entity = tryGet(Scene, 0); return entity != nullptr ? entity->hasComponent<TransformComponent>() : false; }),
			TestFactory::makeStateful<Scene>("access component", true, [](Scene & Scene) { auto entity = tryGet(Scene, 0); auto tc = entity != nullptr ? entity->getComponent<TransformComponent>() : nullptr; return tc != nullptr; })
		)),
		TestFactory::makeStatefulSuite("single entity w/ 2 components", SceneBuilder().addEntity<TransformComponent, UpdateComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entity", (size_t)1, [](Scene & Scene) { return Scene.getRoot().getChildren().size(); }),
			TestFactory::makeStateful<Scene>("has 1st component", true, [](Scene & Scene) { auto entity = tryGet(Scene, 0); return entity->hasComponent<TransformComponent>(); }),
			TestFactory::makeStateful<Scene>("has 2nd component", true, [](Scene & Scene) { auto entity = tryGet(Scene, 0); return entity->hasComponent<UpdateComponent>(); })
		)),
		TestFactory::makeStatefulSuite("2 entities w/ different components", SceneBuilder().addEntity<TransformComponent>().addEntity<UpdateComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entities", (size_t)2, [](Scene & Scene) { return Scene.getRoot().getChildren().size(); }),
			TestFactory::makeStateful<Scene>("1st entity has component", true, [](Scene & Scene) { auto entity = tryGet(Scene, 0); return entity->hasComponent<TransformComponent>(); }),
			TestFactory::makeStateful<Scene>("2nd entity has component", true, [](Scene & Scene) { auto entity = tryGet(Scene, 1); return entity->hasComponent<UpdateComponent>(); })
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
void SceneTestNamespace::SceneBuilder::AddHelper<ComponentType, ComponentTypes...>::add(Scene & Scene, Entity & entity)
{
	entity.addComponent<ComponentType>();
	AddHelper<ComponentTypes...>::add(Scene, entity);
}

void SceneTestNamespace::SceneBuilder::AddHelper<>::add(Scene & /*Scene*/, Entity & /*entity*/)
{
}

void SceneTestNamespace::SceneBuilder::reset()
{
	m_scene = std::make_unique<Scene>();
}

Lent_Ptr<Entity> SceneTestNamespace::tryGet(Scene & Scene, const uint32 index)
{
	auto const & entities = Scene.getRoot().getChildren();
	return index < entities.size() ? entities[index].ptr() : nullptr;
}
