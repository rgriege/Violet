// ============================================================================

#include "game/pathfinding/pathfindingsystem.h"

#include "violet/engine.h"
#include "violet/system/systemfactory.h"
#include "game/world/worldsystem.h"
#include "game/pathfinding/mapcomponent.h"
#include "game/pathfinding/pathcomponent.h"

// ============================================================================

namespace PathfindingSystemNamespace
{
	bool updateComponent(PathComponent & pc, float dt, Violet::Engine & Engine);
}

using namespace PathfindingSystemNamespace;

// ============================================================================

const char * PathfindingSystem::getStaticLabel()
{
	return "path";
}

// ----------------------------------------------------------------------------

void PathfindingSystem::install(Violet::SystemFactory & Factory)
{
	Factory.assign(getStaticLabel(), &PathfindingSystem::init);
}

// ----------------------------------------------------------------------------

void PathfindingSystem::init(Violet::Deserializer & Deserializer)
{
	Deserializer.enterSegment(getStaticLabel());

	Violet::Engine::getInstance().addWriteTask(Violet::Engine::getInstance(),
		[=](Violet::Engine & Engine)
		{
			Engine.addSystem(std::unique_ptr<System>(new PathfindingSystem));
		});
}

// ============================================================================

PathfindingSystem::PathfindingSystem(PathfindingSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

PathfindingSystem::~PathfindingSystem()
{
}

// ----------------------------------------------------------------------------

void PathfindingSystem::update(const float dt)
{
	/*Violet::SceneProcessor processor;
	processor.addDelegate(Violet::SceneProcessor::Filter::create<PathComponent>(), [&](const Violet::Entity & entity, const float dt) {
		PathComponent & pathComponent = *entity.getComponent<PathComponent>();
		if (!updateComponent(pathComponent, dt, Engine))
			Engine.getCurrentScene().getRoot().removeChild(entity);
	});
	processor.process(Engine.getCurrentScene(), dt);*/
}

// ----------------------------------------------------------------------------

void PathfindingSystem::path(Violet::Scene & Scene, Violet::Entity & entity, const Vec2f & goal)
{
	/*const auto & pathfindingComponent = entity.getComponent<PathfindingComponent>();
	if (pathfindingComponent == nullptr)
		return;

	const auto & transformComponent = entity.getComponent<Violet::TransformComponent>();
	if (transformComponent == nullptr)
		return;

	auto view = Scene.getView<MapComponent>();
	auto it = std::find_if(view.begin(), view.end(), [&](std::tuple<MapComponent&> & tuple) { return std::get<0>(tuple).getEntity() == pc->m_mapEntity; });
	if (it != view.end())
		Scene.createComponent<PathComponent>(entity, pc->m_speed, Path::create(tc->m_position, goal, std::get<0>(*it).m_graph));*/
}

// ============================================================================

PathfindingSystem::PathfindingSystem() :
	System(getStaticLabel())
{
}

// ----------------------------------------------------------------------------

/*void PathfindingSystem::createMap(Violet::Deserializer & Deserializer, Violet::Engine & Engine)
{
	m_map = Map(Deserializer);

	for (auto const & road : m_map.getGraph().getEdges())
	{
		Violet::Entity & entity = Engine.getCurrentScene().createEntity();
		auto const & start = m_map.getGraph().getNode(road.m_src).m_position;
		auto const & end = m_map.getGraph().getNode(road.m_destination).m_position;
		auto const center = (end + start) / 2.f;
		auto const halfEdge = (end - start) / 2.f;
		auto const dir = halfEdge.getUnit();
		auto const offset = dir * -5.f + dir.perpendicular() * 5.f;
		Violet::Polygon p{ {
			-halfEdge + offset,
			-halfEdge + offset.perpendicular(),
			halfEdge - offset,
			halfEdge - offset.perpendicular()
		} };
		Engine.getCurrentScene().createComponent<Violet::TransformComponent>(entity, center, 0.f);
		Engine.getCurrentScene().createComponent<Violet::MeshComponent>(entity, p, Violet::Color(128, 128, 128), Violet::ShaderProgram::getCache().fetch("Poly"));
	}
}*/

// ============================================================================

bool PathfindingSystemNamespace::updateComponent(PathComponent & pc, const float dt, Violet::Engine & Engine)
{
	/*auto & tc = pc.getOwner().getComponent<Violet::TransformComponent>();
	auto const & points = pc.m_path.getPoints();
	const Vec2f roadVec = points[pc.m_lastIntersection + 1] - points[pc.m_lastIntersection];
	const Vec2f roadDir = roadVec.getUnit();
	const float roadLen = roadVec.magnitude();
	const Vec2f deltaPos = pc.m_speed * dt * roadDir * Engine.getSystem<WorldSystem>()->getTimeScale();
	const Vec2f nextPos = tc->m_position + deltaPos;

	const Vec2f overshoot = nextPos - points[pc.m_lastIntersection + 1];
	if (overshoot.dot(roadDir) >= 0)
	{
		tc->m_position = points[++pc.m_lastIntersection];
		if (pc.m_lastIntersection < points.size() - 1)
			return updateComponent(pc, dt * overshoot.magnitude() / deltaPos.magnitude(), Engine);
		else
			return false;
	}
	else
		tc->m_position = nextPos;*/
	return true;
}

// ============================================================================
