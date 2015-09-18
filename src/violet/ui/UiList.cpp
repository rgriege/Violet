// ============================================================================

#include "violet/ui/UiList.h"

#include "violet/Engine.h"
#include "violet/input/system/InputSystem.h"
#include "violet/ui/UiListElementComponent.h"
#include "violet/script/ScriptComponent.h"
#include "violet/serialization/file/FileDeserializerFactory.h"

#include <functional>

using namespace Violet;

// ============================================================================

UiList::UiList(const char * const elementFileName, const uint32 elementHeight) :
	m_elementFileName(elementFileName),
	m_elementHeight(elementHeight)
{
}

// ----------------------------------------------------------------------------

void UiList::update(const Entity & entity, const Engine & engine, const uint32 elementCount)
{
	const auto & children = entity.getChildren();
	const auto childCount = children.size();
	const auto & elementFileName = m_elementFileName;
	const uint32 elementHeight = m_elementHeight;

	for (uint32 i = childCount; i < elementCount; ++i)
	{
		engine.addWriteTask(entity, [&elementFileName, i, elementHeight, &engine](Entity & entity)
		{
			auto const deserializer = FileDeserializerFactory::getInstance().create(elementFileName.c_str());
			if (deserializer != nullptr)
			{
				auto child = make_unique_val<Entity>(entity.getScene(), *deserializer);
				auto transform = child->getComponent<TransformComponent>();
				if (transform != nullptr)
					transform->m_transform[1][2] -= i * elementHeight;

				child->addComponent<UiListElementComponent>(i);
				auto script = child->getComponent<ScriptComponent>();
				if (script != nullptr)
				{
					uint32 index = i;
					AssignIndexMethod::run(*script->m_script, child, engine, std::move(index));
				}

				entity.addChild(std::move(child));
			}
		});
	}

	for (uint32 i = elementCount; i < childCount; ++i)
	{
		// engine.addWriteTask(entity, [i](Entity & entity) { entity.removeChild(entity.getChildren()[i]->getHandle()); 
	}
}

// ----------------------------------------------------------------------------

void UiList::clean(Script & script)
{
	MouseDownMethod::remove(script);
}

// ============================================================================
