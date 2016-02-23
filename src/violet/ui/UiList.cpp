// ============================================================================

#include "violet/ui/uilist.h"

#include "violet/engine.h"
#include "violet/input/system/inputsystem.h"
#include "violet/script/scriptcomponent.h"
#include "violet/serialization/file/filedeserializerfactory.h"

#include <functional>

using namespace Violet;

// ============================================================================

UiList::UiList(const char * const elementFileName, const uint32 elementHeight) :
	m_elementFileName(elementFileName),
	m_elementHeight(elementHeight)
{
}

// ----------------------------------------------------------------------------

void UiList::update(const Entity & entity, const uint32 elementCount)
{
	const auto & children = entity.getChildren();
	const auto childCount = children.size();
	const auto & elementFileName = m_elementFileName;
	const uint32 elementHeight = m_elementHeight;

	for (uint32 i = childCount; i < elementCount; ++i)
	{
		Engine::getInstance().addWriteTask(entity, [&elementFileName, i, elementHeight](Entity & entity)
		{
			auto const Deserializer = FileDeserializerFactory::getInstance().create(elementFileName.c_str());
			if (Deserializer != nullptr)
			{
				auto child = make_unique_val<Entity>(entity.getScene(), *Deserializer);
				auto Transform = child->getComponent<TransformComponent>();
				if (Transform != nullptr)
					Transform->m_transform[1][2] -= i * elementHeight;

				auto Script = child->getComponent<ScriptComponent>();
				if (Script != nullptr)
				{
					uint32 index = i;
					AssignIndexMethod::run(*Script->m_script, child, std::move(index));
				}

				entity.addChild(std::move(child));
			}
		});
	}

	for (uint32 i = elementCount; i < childCount; ++i)
	{
		// Engine.addWriteTask(entity, [i](Entity & entity) { entity.removeChild(entity.getChildren()[i]->getHandle()); 
	}
}

// ----------------------------------------------------------------------------

void UiList::clean(Script & Script)
{
	MouseDownMethod::remove(Script);
}

// ============================================================================
