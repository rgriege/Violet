#ifndef VIOLET_EditorComponentWrapper_H
#define VIOLET_EditorComponentWrapper_H

#include "violet/component/Component.h"
#include "violet/component/ComponentDeserializer.h"
#include "violet/task/Thread.h"

namespace edt
{
	template <typename ComponentType>
	class EditorComponentWrapper : public Violet::ComponentBase<EditorComponentWrapper<ComponentType>>
	{
	public:

		static Violet::Tag getStaticTag()
		{
			Violet::Tag s_tag((std::string("e") + ComponentType::getStaticTag().asString()).c_str());
			return s_tag;
		}

		static Violet::Thread getStaticThread()
		{
			return ComponentType::getStaticThread();
		}

	public:

		EditorComponentWrapper(const Violet::EntityId entityId, Violet::ComponentDeserializer & deserializer) :
			ComponentBase<EditorComponentWrapper<ComponentType>>(entityId),
			m_data(entityId, deserializer)
		{
		}

		EditorComponentWrapper(EditorComponentWrapper && other) :
			ComponentBase<EditorComponentWrapper<ComponentType>>(std::move(other)),
			m_data(std::move(other.m_data))
		{
		}

		EditorComponentWrapper(const Violet::EntityId entityId, ComponentType && other) :
			ComponentBase<EditorComponentWrapper<ComponentType>>(entityId),
			m_data(std::move(other))
		{
		}

		ComponentType m_data;
	};

	template <typename ComponentType>
	Violet::Serializer & operator<<(Violet::Serializer & serializer, const EditorComponentWrapper<ComponentType> & component)
	{
		return serializer << component.m_data;
	}
}

#endif
