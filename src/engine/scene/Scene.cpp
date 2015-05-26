// ============================================================================

#include "engine/scene/Scene.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/file/FileDeserializerFactory.h"

#include <iostream>

using namespace Violet;

// ============================================================================

Factory<std::string, void(Scene &, Entity, Deserializer &)> Scene::ms_componentFactory;

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
			scene.createEntity(*deserializer);
	}

	return scene;
}

// ============================================================================

Scene::Scene() :
	m_componentManager(),
	m_entityManager()
{
}

// ----------------------------------------------------------------------------

Scene::~Scene()
{
}

// ----------------------------------------------------------------------------

Scene::Scene(Scene && other) :
	m_componentManager(std::move(other.m_componentManager)),
	m_entityManager(std::move(other.m_entityManager))
{
}

// ----------------------------------------------------------------------------

Scene & Scene::operator=(Scene && other)
{
	std::swap(m_componentManager, other.m_componentManager);
	std::swap(m_entityManager, other.m_entityManager);
	return *this;
}

// ----------------------------------------------------------------------------

Entity Scene::createEntity()
{
	return m_entityManager.create();
}

// ----------------------------------------------------------------------------

Entity Scene::createEntity(Deserializer & deserializer)
{
	auto entitySegment = deserializer.enterSegment("ntty");
	const bool referenced = entitySegment->getBoolean("ref");
	const Entity entity = referenced ? m_entityManager.create(entitySegment->getUint("id")) : m_entityManager.create();
	while (*entitySegment)
	{
		std::string label = entitySegment->nextLabel();
		auto componentSegment = entitySegment->enterSegment(label.c_str());
		ms_componentFactory.create(label.c_str(), *this, entity, *componentSegment);
	}
	return entity;
}

// ----------------------------------------------------------------------------

void Scene::destroyEntity(const Entity entity)
{
	m_entityManager.destroy(entity);
	m_componentManager.removeAll(entity);
}

// ----------------------------------------------------------------------------

std::vector<Entity> Scene::getEntities() const
{
	return m_entityManager.getEntities();
}

// ----------------------------------------------------------------------------

void Scene::clear()
{
	for (auto entity : getEntities())
		destroyEntity(entity);
}

// ============================================================================
