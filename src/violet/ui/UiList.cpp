// ============================================================================

#include "violet/ui/UiList.h"

#include "violet/Engine.h"
#include "violet/input/system/InputSystem.h"

#include <functional>

using namespace Violet;

// ============================================================================

UiList::UiList(const char * const elementFileName) :
	m_elementFileName(elementFileName)
{
}

// ----------------------------------------------------------------------------

void UiList::update(const Entity & entity, const Engine & engine, const uint32 elementCount)
{
	const auto & children = entity.getChildren();
	const auto childCount = children.size();
	const auto & elementFileName = m_elementFileName;

	for (uint32 i = childCount; i < elementCount; ++i)
	{
		engine.addWriteTask(entity, [&elementFileName, i](Entity & entity)
		{
			auto const deserializer = FileDeserializerFactory::getInstance().create(elementFileName.c_str());
			if (deserializer != nullptr)
			{
				auto child = make_unique_val<Entity>(entity.getScene(), *deserializer);
				auto & transform = child->getComponent<TransformComponent>();
				if (transform != nullptr)
					transform->m_transform[1][2] += i * 20;
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
