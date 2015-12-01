// ============================================================================

#include "violet/scene/Scene.h"

#include "violet/Engine.h"
#include "violet/entity/Entity.h"
#include "violet/log/Log.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

// ============================================================================

std::unique_ptr<Scene, void(*)(Scene *)> Scene::create(const char * filename)
{
	auto scene = std::unique_ptr<Scene, void(*)(Scene *)>(new Scene, Scene::destroy);

	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
		Log::log(FormattedString<128>().sprintf("Could not open scene file '%s'", filename));
	else if (!*deserializer)
		Log::log(FormattedString<128>().sprintf("Failed to parse scene file '%s'", filename));
	else
		scene->m_root = std::make_unique<Entity>(*scene, *deserializer);

	return scene;
}

// ----------------------------------------------------------------------------

void Scene::destroy(Scene * scene)
{
	if (scene != nullptr)
	{
		scene->m_root.reset();
		Engine::getInstance().addDeleteTask(std::make_unique<DelegateTask>([=]() { delete scene; }));
	}
}

// ============================================================================

Scene::Scene() :
	m_lookupMap(),
	m_handleManager(),
	m_root(std::make_unique<Entity>(*this))
{
}

// ----------------------------------------------------------------------------

const Entity & Scene::getRoot() const
{
	return *m_root;
}

// ----------------------------------------------------------------------------

lent_ptr<const Entity> Scene::getEntity(const Handle handle) const
{
	const auto it = m_lookupMap.find(handle);
	return it != m_lookupMap.end() ? &it->second.get() : nullptr;
}

// ----------------------------------------------------------------------------

void Scene::index(const HandleComponent & handleComponent) thread_const
{
	Engine::getInstance().addWriteTask(*this,
		[&](Scene & scene)
		{
			const Handle handle = handleComponent.getHandle().isValid() ? scene.m_handleManager.create(handleComponent.getHandle().getId()) : scene.m_handleManager.create();
			scene.m_lookupMap.emplace(handle, handleComponent.getOwner());

			Engine::getInstance().addWriteTask(handleComponent,
				[handle](HandleComponent & handleComponent)
			{
				handleComponent.m_handle = handle;
			});
		});
}

// ----------------------------------------------------------------------------

void Scene::deindex(const HandleComponent & handleComponent) thread_const
{
	assert(m_lookupMap.find(handleComponent.getHandle()) != m_lookupMap.end() && &m_lookupMap.find(handleComponent.getHandle())->second.get() == &handleComponent.getOwner());

	const Handle handle = handleComponent.getHandle();
	Engine::getInstance().addWriteTask(*this,
		[=](Scene & scene)
		{
			scene.m_handleManager.free(handle);
			scene.m_lookupMap.erase(handle);
		});
}

// ============================================================================

Scene::~Scene()
{
	assert(m_root == nullptr);
	assert(m_lookupMap.empty());
}

// ============================================================================
