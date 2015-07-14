// ============================================================================

#include "engine/scene/Scene.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/file/FileDeserializerFactory.h"

#include <iostream>

using namespace Violet;

// ============================================================================

Factory<std::string, void(Scene &, Entity &, Deserializer &)> Scene::ms_componentFactory;

// ============================================================================

Scene Scene::create(const char * filename)
{
	Scene scene;

	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
		std::cout << "Could not open scene file " << filename << std::endl;
	else if (!*deserializer)
		std::cout << "Failed to parse scene file " << filename << std::endl;
	else
	{
		while (*deserializer)
			scene.createEntity(scene.m_root, *deserializer);
	}

	return scene;
}

// ============================================================================

Scene::Scene() :
	m_root(Handle::ms_invalid),
	m_lookupMap(),
	m_handleManager()
{
	m_root.addToScene(*this);
}

// ----------------------------------------------------------------------------

Scene::~Scene()
{
	if (m_root.inScene())
		m_root.removeFromScene();
}

// ----------------------------------------------------------------------------

Scene::Scene(Scene && other) :
	m_root(std::move(other.m_root)),
	m_lookupMap(std::move(other.m_lookupMap)),
	m_handleManager(std::move(other.m_handleManager))
{
	if (m_root.inScene())
	{
		m_root.removeFromScene();
		m_root.addToScene(*this);
	}
}

// ----------------------------------------------------------------------------

Scene & Scene::operator=(Scene && other)
{
	m_root = std::move(other.m_root);
	m_lookupMap = std::move(other.m_lookupMap);
	m_handleManager = std::move(other.m_handleManager);

	if (m_root.inScene())
	{
		m_root.removeFromScene();
		m_root.addToScene(*this);
	}

	return *this;
}

// ----------------------------------------------------------------------------

Entity & Scene::createEntity(Entity & parent)
{
	Entity & entity = parent.addChild(Entity(m_handleManager.create()));
	return entity;
}

// ----------------------------------------------------------------------------

Entity & Scene::createEntity(Entity & parent, Deserializer & deserializer)
{
	auto entitySegment = deserializer.enterSegment("ntty");
	const bool referenced = entitySegment->getBoolean("ref");
	Entity entity(referenced ? m_handleManager.create(entitySegment->getUint("id")) : m_handleManager.create());
	auto componentsSegment = entitySegment->enterSegment("cpnt");
	while (*componentsSegment)
	{
		const char * const label = componentsSegment->nextLabel();
		auto componentSegment = componentsSegment->enterSegment(label);
		ms_componentFactory.create(label, *this, entity, *componentSegment);
	}

	auto childrenSegment = entitySegment->enterSegment("chld");
	while (*childrenSegment)
		createEntity(entity, *childrenSegment);

	Entity & result = parent.addChild(std::move(entity));
	return result;
}

// ----------------------------------------------------------------------------

Entity & Scene::getRoot()
{
	return m_root;
}

// ----------------------------------------------------------------------------

const Entity & Scene::getRoot() const
{
	return m_root;
}

// ----------------------------------------------------------------------------

Entity & Scene::getEntity(const Handle handle)
{
	static Entity s_invalidEntity(Handle::ms_invalid);
	const auto it = m_lookupMap.find(handle);
	return it != m_lookupMap.end() ? it->second : s_invalidEntity;
}

// ----------------------------------------------------------------------------

const Entity & Scene::getEntity(const Handle handle) const
{
	const auto it = m_lookupMap.find(handle);
	return it != m_lookupMap.end() ? it->second : Entity::ms_invalid;
}

// ----------------------------------------------------------------------------

bool Scene::destroyEntity(const Handle handle)
{
	// TODO - implement
	const auto it = m_lookupMap.find(handle);
	const bool found = it != m_lookupMap.end();

	if (found)
	{
		Entity & entity = it->second;
		m_lookupMap.erase(it);
		entity.removeFromScene();
		entity.getParent()->removeChild(handle);
		m_handleManager.free(handle);
	}

	return found;
}

// ----------------------------------------------------------------------------

void Scene::index(Entity & entity)
{
	m_lookupMap.emplace(entity.getHandle(), entity);
}

// ----------------------------------------------------------------------------

void Scene::reindex(Entity & entity)
{
	m_lookupMap.find(entity.getHandle())->second = entity;
}

// ----------------------------------------------------------------------------

bool Scene::deindex(const Handle handle)
{
	return m_lookupMap.erase(handle) > 0;
}

// ============================================================================
