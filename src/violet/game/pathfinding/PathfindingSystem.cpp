#include "violet/game/pathfinding/PathfindingSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/entity/EntityFactory.h"
#include "violet/core/math/Polygon.h"
#include "violet/core/transform/TransformComponent.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/plugins/graphics/system/RenderSystem.h"
#include "violet/plugins/graphics/shader/Shader.h"

namespace PathfindingSystemNamespace
{
	bool updateComponent(PathComponent & pc, float dt, Engine & engine);
}

using namespace PathfindingSystemNamespace;

void PathfindingSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &PathfindingSystem::init);
}

std::unique_ptr<System> PathfindingSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	return std::unique_ptr<System>(new PathfindingSystem);
}

PathfindingSystem::~PathfindingSystem()
{
	EntityFactory::getInstance().remove("map");
}

void PathfindingSystem::update(const float dt, Engine & engine)
{
	for (auto it = getComponents().begin(), end = getComponents().end(); it != end; )
	{
		if (!updateComponent(*it, dt, engine))
		{
			it = remove(it->getEntity());
			end = getComponents().end();
		}
		else
			++it;
	}
}

Path PathfindingSystem::getPath(const Vec2f & start, const Vec2f & goal)
{
	return Path::create(start, goal, m_map);
}

PathfindingSystem::PathfindingSystem() :
	m_map()
{
	EntityFactory::getInstance().assign("map", std::bind(&PathfindingSystem::createMap, this, std::placeholders::_1, std::placeholders::_2));
}

void PathfindingSystem::createMap(Deserializer & deserializer, SceneInitContext & initContext)
{
	m_map = Map(deserializer);

	for (auto const & road : m_map.getGraph().getEdges())
	{
		Entity & entity = initContext.createEntity();
		auto const & start = m_map.getGraph().getNode(road.m_src).m_position;
		auto const & end = m_map.getGraph().getNode(road.m_destination).m_position;
		auto const center = (end + start) / 2.f;
		auto const halfEdge = (end - start) / 2.f;
		auto const dir = halfEdge.getUnit();
		auto const offset = dir * -5.f + dir.perpendicular() * 5.f;
		Polygon p{ {
			-halfEdge + offset,
			-halfEdge + offset.perpendicular(),
			halfEdge - offset,
			halfEdge - offset.perpendicular()
		} };
		initContext.createComponent<TransformSystem, TransformComponent>(entity, center, 0.f);
		initContext.createComponent<RenderSystem, RenderComponent>(entity, p, Color(128, 128, 128), ShaderProgram::getCache().fetch("poly"));
	}
}

bool PathfindingSystemNamespace::updateComponent(PathComponent & pc, const float dt, Engine & engine)
{
 	auto & tc = engine.fetch<TransformComponent>(pc.getEntity());
	auto const & points = pc.m_path.getPoints();
	const Vec2f roadVec = points[pc.m_lastIntersection + 1] - points[pc.m_lastIntersection];
	const Vec2f roadDir = roadVec.getUnit();
	const float roadLen = roadVec.magnitude();
	const Vec2f deltaPos = pc.m_speed * dt * roadDir;
	const Vec2f nextPos = tc.m_position + deltaPos;

	const Vec2f overshoot = nextPos - points[pc.m_lastIntersection + 1];
	if (overshoot.dot(roadDir) >= 0)
	{
		if (++pc.m_lastIntersection < points.size() - 1)
		{
			tc.m_position = points[pc.m_lastIntersection];
			return updateComponent(pc, dt * overshoot.magnitude() / deltaPos.magnitude(), engine);
		}
		else
			return false;
	}
	else
		tc.m_position = nextPos;
	return true;
}