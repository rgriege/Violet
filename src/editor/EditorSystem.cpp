// ============================================================================

#include "editor/EditorSystem.h"

#include "editor/EditorComponent.h"
#include "engine/Engine.h"
#include "engine/scene/Scene.h"
#include "engine/serialization/Deserializer.h"
#include "engine/system/SystemFactory.h"

using namespace Violet;

// ============================================================================

namespace EditorSystemNamespace
{
}

using namespace EditorSystemNamespace;

// ============================================================================

const char * EditorSystem::getStaticLabel()
{
	return "edtr";
}

// ----------------------------------------------------------------------------

void EditorSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &EditorSystem::init);
}

// ----------------------------------------------------------------------------

std::unique_ptr<System> EditorSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());
	return std::unique_ptr<EditorSystem>(new EditorSystem);
}

// ============================================================================

void EditorSystem::update(const float /*dt*/, const Engine & /*engine*/)
{
}

// ----------------------------------------------------------------------------

void EditorSystem::loadScene(const char * const filename, const Engine & engine) const
{
	if (m_scene != nullptr)
	{
		// cleanup
		assert(false);
	}

	m_scene = Scene::create(filename);

	/*if (m_scene != nullptr)
	{
		for (auto const & child : m_scene->getRoot().getChildren())
		{
			engine.getCurrentScene().getEntity(Handle(0))->addChild();
			child->addComponent<EditorComponent>(Handle(0, 0));
		}
	}*/
}

// ----------------------------------------------------------------------------

const std::unique_ptr<Scene> & EditorSystem::getScene() const
{
	return m_scene;
}

// ============================================================================

EditorSystem::EditorSystem() :
	System(getStaticLabel()),
	m_scene(nullptr)
{
}

// ============================================================================
