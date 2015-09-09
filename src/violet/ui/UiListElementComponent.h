#ifndef VIOLET_UiListElementComponent_H
#define VIOLET_UiListElementComponent_H

#include "violet/component/Component.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UiListElementComponent : public ComponentBase<UiListElementComponent>
	{
	public:

		static Tag getStaticTag();

	public:

		UiListElementComponent(const Entity & owner, uint32 index);
		UiListElementComponent(UiListElementComponent && other);

	public:

		const uint32 m_index;
	};
}

#endif