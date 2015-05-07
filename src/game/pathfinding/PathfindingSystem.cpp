// ============================================================================

#include "game/pathfinding/PathfindingSystem.h"

#include "engine/Engine.h"
#include "engine/system/SystemFactory.h"
#include "engine/transform/component/TransformComponent.h"
#include "game/world/WorldSystem.h"
#include "game/pathfinding/MapComponent.h"
#include "game/pathfinding/PathComponent.h"

// ============================================================================

namespace PathfindingSystemNamespace
{
	bool updateComponent(PathComponent & pc, float dt, Violet::Engine & engine);
}

using namespace PathfindingSystemNamespace;

// ============================================================================

const char * PathfindingSystem::getStaticLabel()
{
	return "path";
}

// ----------------------------------------------------------------------------

void PathfindingSystem::install(Violet::SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &PathfindingSystem::init);
}

// ----------------------------------------------------------------------------

std::unique_ptr<Violet::System> PathfindingSystem::init(Violet::Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	return std::unique_ptr<Violet::System>(new PathfindingSystem);
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

void PathfindingSystem::update(const float dt, Violet::Engine & engine)
{
	for (auto & componentSet : engine.getCurrentScene().getView<PathComponent>())
	{
		PathComponent & component = Violet::get<PathComponent&>(componentSet);
		if (!updateComponent(component, dt, engine))
			engine.getCurrentScene().removeComponent<PathComponent>(component.getEntity());
	}
}

// ----------------------------------------------------------------------------

void PathfindingSystem::path(Violet::Scene & scene, const Violet::Entity entity, const Vec2f & goal)
{
	const PathfindingComponent * pc = scene.getComponent<PathfindingComponent>(entity);
	if (pc == nullptr)
		return;

	const Violet::TransformComponent * tc = scene.getComponent<Violet::TransformComponent>(entity);
	if (tc == nullptr)
		return;

	auto view = scene.getView<MapComponent>();
	auto it = std::find_if(view.begin(), view.end(), [&](std::tuple<MapComponent&> & tuple) { return std::get<0>(tuple).getEntity() == pc->m_mapEntity; });
	if (it != view.end())
		scene.createComponent<PathComponent>(entity, pc->m_speed, Path::create(tc->m_position, goal, std::get<0>(*it).m_graph));
}

// ============================================================================

PathfindingSystem::PathfindingSystem() :
	System(getStaticLabel())
{
}

// ----------------------------------------------------------------------------

/*void PathfindingSystem::createMap(Violet::Deserializer & deserializer, Violet::Engine & engine)
{
	m_map = Map(deserializer);

	for (auto const & road : m_map.getGraph().getEdges())
	{
		Violet::Entity & entity = engine.getCurrentScene().createEntity();
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
		engine.getCurrentScene().createComponent<Violet::TransformComponent>(entity, center, 0.f);
		engine.getCurrentScene().createComponent<Violet::RenderComponent>(entity, p, Violet::Color(128, 128, 128), Violet::ShaderProgram::getCache().fetch("poly"));
	}
}*/

// ============================================================================

bool PathfindingSystemNamespace::updateComponent(PathComponent & pc, const float dt, Violet::Engine & engine)
{
	auto tc = engine.getCurrentScene().getComponent<Violet::TransformComponent>(pc.getEntity());
	auto const & points = pc.m_path.getPoints();
	const Vec2f roadVec = points[pc.m_lastIntersection + 1] - points[pc.m_lastIntersection];
	const Vec2f roadDir = roadVec.getUnit();
	const float roadLen = roadVec.magnitude();
	const Vec2f deltaPos = pc.m_speed * dt * roadDir * engine.fetch<WorldSystem>()->getTimeScale();
	const Vec2f nextPos = tc->m_position + deltaPos;

	const Vec2f overshoot = nextPos - points[pc.m_lastIntersection + 1];
	if (overshoot.dot(roadDir) >= 0)
	{
		tc->m_position = points[++pc.m_lastIntersection];
		if (pc.m_lastIntersection < points.size() - 1)
			return updateComponent(pc, dt * overshoot.magnitude() / deltaPos.magnitude(), engine);
		else
			return false;
	}
	else
		tc->m_position = nextPos;
	return true;
}

// ============================================================================
