#ifndef EDITOR_EditorComponent_H
#define EDITOR_EditorComponent_H

#include "editor/Defines.h"
#include "violet/component/Component.h"
#include "violet/task/Thread.h"

namespace Violet
{
	class ComponentDeserializer;
	class Serializer;
}

namespace edt
{
	class EDITOR_API EditorComponent : public Violet::ComponentBase<EditorComponent, 0>
	{
	public:

		static Violet::Tag getStaticTag();
		static Violet::Thread getStaticThread();

	public:

		EditorComponent(Violet::EntityId entityId, Violet::EntityId editId);
		EditorComponent(Violet::EntityId entityId, const Violet::ComponentDeserializer & deserializer);
		EditorComponent(EditorComponent && other);

	public:

		Violet::EntityId m_editId;
	};

	EDITOR_API Violet::Serializer & operator<<(Violet::Serializer & serializer, const EditorComponent & component);
}

#endif
