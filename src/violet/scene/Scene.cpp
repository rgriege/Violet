// ============================================================================

#include "violet/scene/Scene.h"

#include "violet/entity/Entity.h"
#include "violet/log/Log.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

// ============================================================================

std::unique_ptr<Scene> Scene::create(const char * filename)
{
	auto scene = std::make_unique<Scene>();

	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
		Log::log(FormattedString<128>().sprintf("Could not open scene file '%s'", filename));
	else if (!*deserializer)
		Log::log(FormattedString<128>().sprintf("Failed to parse scene file '%s'", filename));
	else
		scene->m_root = make_unique_val<Entity>(*scene, *deserializer);

	return scene;
}

// ============================================================================

Scene::Scene() :
	m_lookupMap(),
	m_handleManager(),
	m_root(make_unique_val<Entity>(*this)),
	m_eventContext()
{
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

lent_ptr<Entity> Scene::getEntity(const Handle handle)
{
	const auto it = m_lookupMap.find(handle);
	return it != m_lookupMap.end() ? &it->second.get() : nullptr;
}

// ----------------------------------------------------------------------------

lent_ptr<const Entity> Scene::getEntity(const Handle handle) const
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

void Scene::index(HandleComponent & handleComponent)
{
#ifdef _DEBUG
	assert(m_lookupMap.find(handleComponent.getHandle()) == m_lookupMap.end());
#endif

	m_lookupMap.emplace(handleComponent.getHandle(), handleComponent.getOwner());
}

// ----------------------------------------------------------------------------

bool Scene::deindex(HandleComponent & handleComponent)
{
#ifdef _DEBUG
	auto const it = m_lookupMap.find(handleComponent.getHandle());
	assert(it != m_lookupMap.end() && &it->second.get() == &handleComponent.getOwner());
#endif

	m_handleManager.free(handleComponent.getHandle());
	return m_lookupMap.erase(handleComponent.getHandle()) > 0;
}

// ============================================================================
