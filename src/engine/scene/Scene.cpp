// ============================================================================

#include "engine/scene/Scene.h"

#include "engine/entity/Entity.h"
#include "engine/serialization/Deserializer.h"
#include "engine/serialization/file/FileDeserializerFactory.h"

#include <iostream>

using namespace Violet;

// ============================================================================

std::unique_ptr<Scene> Scene::create(const char * filename)
{
	auto scene = std::make_unique<Scene>();

	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
		std::cout << "Could not open scene file " << filename << std::endl;
	else if (!*deserializer)
		std::cout << "Failed to parse scene file " << filename << std::endl;
	else
	{
		while (*deserializer)
			scene->m_root->addChild(std::make_unique<Entity>(*scene, *deserializer));
	}

	return scene;
}

// ============================================================================

Scene::Scene() :
	m_lookupMap(),
	m_handleManager(),
	m_root()
{
	m_root = std::make_unique<Entity>(*this);
}

// ----------------------------------------------------------------------------

Scene::~Scene()
{
}

// ----------------------------------------------------------------------------

Scene::Scene(Scene && other) :
	m_lookupMap(std::move(other.m_lookupMap)),
	m_handleManager(std::move(other.m_handleManager)),
	m_root(std::move(other.m_root))
{
}

// ----------------------------------------------------------------------------

Entity & Scene::getRoot()
{
	return *m_root;
}

// ----------------------------------------------------------------------------

const Entity & Scene::getRoot() const
{
	return *m_root;
}

// ----------------------------------------------------------------------------

Entity * Scene::getEntity(const Handle handle)
{
	const auto it = m_lookupMap.find(handle);
	return it != m_lookupMap.end() ? &it->second.get() : nullptr;
}

// ----------------------------------------------------------------------------

const Entity * Scene::getEntity(const Handle handle) const
{
	const auto it = m_lookupMap.find(handle);
	return it != m_lookupMap.end() ? &it->second.get() : nullptr;
}

// ----------------------------------------------------------------------------

Handle Scene::createHandle(const Handle desiredHandle)
{
	return desiredHandle.isValid() ? m_handleManager.create(desiredHandle.getId()) : m_handleManager.create();
}

// ----------------------------------------------------------------------------

void Scene::index(Entity & entity)
{
	m_lookupMap.emplace(entity.getHandle(), entity);
}

// ----------------------------------------------------------------------------

bool Scene::deindex(const Handle handle)
{
	m_handleManager.free(handle);
	return m_lookupMap.erase(handle) > 0;
}

// ============================================================================
