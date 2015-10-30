// ============================================================================

#include "editor/EditorSystem.h"

#include "violet/Engine.h"
#include "violet/graphics/component/ColorComponent.h"
#include "violet/graphics/component/TextComponent.h"
#include "violet/graphics/component/TextureComponent.h"
#include "violet/input/component/KeyInputComponent.h"
#include "violet/input/component/MouseInputComponent.h"
#include "violet/scene/Scene.h"
#include "violet/script/ScriptComponent.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/update/component/UpdateComponent.h"

using namespace Violet;

// ============================================================================

namespace EditorSystemNamespace
{
	template <typename ComponentType>
	class EditorComponentWrapper : public ComponentBase<EditorComponentWrapper<ComponentType>>
	{
	public:

		static void create(Entity & entity, Deserializer & deserializer)
		{
			entity.addComponent<EditorComponentWrapper<ComponentType>>(deserializer);
		}

		static Tag getStaticTag() { return Tag('e', 'd', 't', 'r'); }

	public:

		EditorComponentWrapper(Entity & owner, Deserializer & deserializer) :
			ComponentBase<EditorComponentWrapper<ComponentType>>(owner),
			m_data(owner, deserializer)
		{
		}

		EditorComponentWrapper(EditorComponentWrapper && other) :
			ComponentBase<EditorComponentWrapper<ComponentType>>(std::move(other)),
			m_data(std::move(other.m_data))
		{
		}

		ComponentType m_data;
	};

	Polygon getRenderArea(const Entity & entity);

	void prepareForEditor(Entity & entity, std::string const & editScriptFileName);
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
	auto & segment = deserializer.enterSegment(getStaticLabel());
	std::string editScriptFileName = segment->getString("editScriptFileName");
	return std::unique_ptr<EditorSystem>(new EditorSystem(std::move(editScriptFileName)));
}

// ============================================================================

void EditorSystem::update(const float /*dt*/, const Engine & /*engine*/)
{
}

// ----------------------------------------------------------------------------

void EditorSystem::loadScene(const char * const filename, const Engine & engine)
{
	if (m_rootSceneHandle.isValid())
	{
		// cleanup
		assert(false);
	}

	engine.addWriteTask(engine.getCurrentScene().getRoot(), [&](Entity & root, const std::string & sceneFileName, const std::string & editScriptFileName)
		{
			Entity::uninstallComponent<MouseInputComponent>();
			Entity::uninstallComponent<KeyInputComponent>();
			Entity::uninstallComponent<ScriptComponent>();
			Entity::uninstallComponent<UpdateComponent>();

			Entity::installComponent(MouseInputComponent::getStaticTag(), &EditorComponentWrapper<MouseInputComponent>::create);
			Entity::installComponent(KeyInputComponent::getStaticTag(), &EditorComponentWrapper<KeyInputComponent>::create);
			Entity::installComponent(ScriptComponent::getStaticTag(), &EditorComponentWrapper<ScriptComponent>::create);
			Entity::installComponent(UpdateComponent::getStaticTag(), &EditorComponentWrapper<UpdateComponent>::create);

			Entity * newSceneRoot = nullptr;
			auto deserializer = FileDeserializerFactory::getInstance().create(sceneFileName.c_str());
			if (deserializer == nullptr)
				std::cout << "Could not open scene file " << sceneFileName << std::endl;
			else if (!*deserializer)
				std::cout << "Failed to parse scene file " << sceneFileName << std::endl;
			else
				newSceneRoot = &root.addChild(*deserializer);

			Entity::uninstallComponent(MouseInputComponent::getStaticTag());
			Entity::uninstallComponent(KeyInputComponent::getStaticTag());
			Entity::uninstallComponent(ScriptComponent::getStaticTag());
			Entity::uninstallComponent(UpdateComponent::getStaticTag());

			Entity::installComponent<MouseInputComponent>();
			Entity::installComponent<KeyInputComponent>();
			Entity::installComponent<ScriptComponent>();
			Entity::installComponent<UpdateComponent>();

			if (newSceneRoot != nullptr)
			{
				newSceneRoot->addComponent<HandleComponent>();
				m_rootSceneHandle = newSceneRoot->getComponent<HandleComponent>()->getHandle();
				for (auto & child : newSceneRoot->getChildren())
					prepareForEditor(child, editScriptFileName);
			}
		}, std::string(filename), m_editScriptFileName);
}

// ----------------------------------------------------------------------------

bool EditorSystem::hasScene() const
{
	return m_rootSceneHandle.isValid();
}

// ----------------------------------------------------------------------------

const lent_ptr<const Entity> & EditorSystem::getSceneRoot(const Engine & engine) const
{
	return engine.getCurrentScene().getEntity(m_rootSceneHandle);
}

// ============================================================================

EditorSystem::EditorSystem(std::string && editScriptFileName) :
	System(getStaticLabel()),
	m_rootSceneHandle(),
	m_editScriptFileName(std::move(editScriptFileName))
{
}

// ============================================================================

void EditorSystemNamespace::prepareForEditor(Entity & entity, std::string const & editScriptFileName)
{
	if (!editScriptFileName.empty())
	{
		Polygon & renderArea = getRenderArea(entity);
		if (!renderArea.m_vertices.empty())
		{
			entity.addComponent<ScriptComponent>(editScriptFileName.c_str());
			entity.addComponent<MouseInputComponent>(std::move(renderArea));
		}
	}
}

// ----------------------------------------------------------------------------

Polygon EditorSystemNamespace::getRenderArea(const Entity & entity)
{
	if (entity.hasComponent<ColorComponent>())
		return entity.getComponent<ColorComponent>()->m_mesh.getPolygon();
	else if (entity.hasComponent<TextComponent>())
		return entity.getComponent<TextComponent>()->m_mesh.getPolygon();
	else if (entity.hasComponent<TextureComponent>())
		return entity.getComponent<TextureComponent>()->m_mesh.getPolygon();
	else
		return Polygon(std::vector<Vec2f>());
}

// ============================================================================
