#include "test/scene/SceneTest.h"

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
			static void add(Scene & scene, const Entity entity);
		};

		template <>
		struct AddHelper<>
		{
			static void add(Scene & scene, const Entity entity);
		};

	private:
		
		void reset();

	private:

		std::unique_ptr<Scene> m_scene;
	};

	Entity tryGet(Scene & scene, uint32 index);
	ptrdiff_t attempt(Scene & scene)
	{
		auto view = scene.getView<TransformComponent, UpdateComponent>();
		return std::distance(view.begin(), view.end());
	}
}

using namespace SceneTestNamespace;

void SceneTest::run(TestEvaluator & evaluator)
{
	TestFactory::makeStatelessSuite("scene tests", std::forward_as_tuple(
		TestFactory::makeStatefulSuite("empty", SceneBuilder().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("entity check", true, [](Scene & scene) { return scene.getEntities().empty(); }),
			TestFactory::makeStateful<Scene>("view iteration", true, [](Scene & scene) { auto view = scene.getView<TransformComponent>(); return view.begin() == view.end(); })
		)),
		TestFactory::makeStatefulSuite("single entity w/ 1 component", SceneBuilder().addEntity<TransformComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entity", 1u, [](Scene & scene) { return scene.getEntities().size(); }),
			TestFactory::makeStateful<Scene>("has component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return scene.hasComponent<TransformComponent>(entity); }),
			TestFactory::makeStateful<Scene>("access component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); auto tc = scene.getComponent<TransformComponent>(entity); return tc != nullptr; }),
			TestFactory::makeStateful<Scene>("single component view count", 1, [](Scene & scene) { auto view = scene.getView<TransformComponent>(); return std::distance(view.begin(), view.end()); }),
			TestFactory::makeStateful<Scene>("single component view value", Vec2f::ZERO, [](Scene & scene) { auto view = scene.getView<TransformComponent>(); return get<TransformComponent&>(*view.begin()).m_position; })
		)),
		TestFactory::makeStatefulSuite("single entity w/ 2 components", SceneBuilder().addEntity<TransformComponent, UpdateComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entity", 1u, [](Scene & scene) { return scene.getEntities().size(); }),
			TestFactory::makeStateful<Scene>("has 1st component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return scene.hasComponent<TransformComponent>(entity); }),
			TestFactory::makeStateful<Scene>("has 2nd component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return scene.hasComponent<UpdateComponent>(entity); }),
			TestFactory::makeStateful<Scene>("single component view count", 1, [](Scene & scene) { auto view = scene.getView<TransformComponent>(); return std::distance(view.begin(), view.end()); }),
			TestFactory::makeStateful<Scene>("double component view count", 1, [](Scene & scene) { auto view = scene.getView<TransformComponent, UpdateComponent>(); return std::distance(view.begin(), view.end()); })
		)),
		TestFactory::makeStatefulSuite("2 entities w/ different components", SceneBuilder().addEntity<TransformComponent>().addEntity<UpdateComponent>().create(), std::forward_as_tuple(
			TestFactory::makeStateful<Scene>("has entities", 2u, [](Scene & scene) { return scene.getEntities().size(); }),
			TestFactory::makeStateful<Scene>("1st entity has component", true, [](Scene & scene) { auto entity = tryGet(scene, 0); return scene.hasComponent<TransformComponent>(entity); }),
			TestFactory::makeStateful<Scene>("2nd entity has component", true, [](Scene & scene) { auto entity = tryGet(scene, 1); return scene.hasComponent<UpdateComponent>(entity); }),
			TestFactory::makeStateful<Scene>("1st component view count", 1, [](Scene & scene) { auto view = scene.getView<TransformComponent>(); return std::distance(view.begin(), view.end()); }),
			TestFactory::makeStateful<Scene>("2nd component view count", 1, [](Scene & scene) { auto view = scene.getView<UpdateComponent>(); return std::distance(view.begin(), view.end()); }),
			TestFactory::makeStateful<Scene>("double component view count", 0, attempt)
			//TestFactory::makeStateful<Scene>("double component view count", 0, [](Scene & scene) { auto view = scene.getView<TransformComponent, UpdateComponent>(); return std::distance(view.begin(), view.end()); })
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
	const Entity entity = m_scene->createEntity();
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
void SceneTestNamespace::SceneBuilder::AddHelper<ComponentType, ComponentTypes...>::add(Scene & scene, const Entity entity)
{
	scene.createComponent<ComponentType>(entity);
	AddHelper<ComponentTypes...>::add(scene, entity);
}

void SceneTestNamespace::SceneBuilder::AddHelper<>::add(Scene & /*scene*/, const Entity /*entity*/)
{
}

void SceneTestNamespace::SceneBuilder::reset()
{
	m_scene = std::make_unique<Scene>();
}

Entity SceneTestNamespace::tryGet(Scene & scene, const uint32 index)
{
	auto const & entities = scene.getEntities();
	return index < entities.size() ? entities[index] : Entity::ms_invalid;
}