#ifndef VIOLET_EditorComponent_H
#define VIOLET_EditorComponent_H

#include "violet/component/Component.h"
#include "violet/utility/handle/Handle.h"

#include "editor/EditorConfig.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_EDITOR_API EditorComponent : public ComponentBase<EditorComponent>
	{
	public:

		static Tag getStaticTag() { return Tag('e', 'd', 't', 'r'); }

	public:

		EditorComponent(const Entity & owner, Handle handle);
		EditorComponent(EditorComponent && other);

	public:

		Handle m_handle;
	};

	Deserializer & operator>>(Deserializer & deserializer, EditorComponent & component);
	Serializer & operator<<(Serializer & serializer, const EditorComponent & component);
}

#endif