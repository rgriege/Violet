// ============================================================================

#include "violet/scene/Scene.h"

#include "violet/Engine.h"
#include "violet/entity/Entity.h"
#include "violet/handle/HandleManager.h"
#include "violet/log/Log.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

// ============================================================================

std::unique_ptr<Scene> Scene::create(const char * const name)
{
	auto scene = std::unique_ptr<Scene>(new Scene);
	scene->m_componentManager->load(name);
	scene->m_handleManager.create(0);
	return scene;
}

// ============================================================================

Scene::Scene() :
	m_handleManager(),
	m_componentManager(std::make_unique<ComponentManager>())
{
}

// ----------------------------------------------------------------------------

/*Scene::Scene(Scene && rhs) :
	m_handleManager(std::move(rhs.m_handleManager)),
	m_componentManager(std::move(rhs.m_componentManager))
{
}*/

// ----------------------------------------------------------------------------

void Scene::loadEntity(const char * entityName)
{
	m_componentManager->loadEntity(m_handleManager.create(), entityName);
}

// ----------------------------------------------------------------------------

void Scene::save(Serializer & serializer) const
{
}

// ============================================================================

Scene::~Scene()
{
}

// ============================================================================
