#ifndef VIOLET_UiStateComponent_H
#define VIOLET_UiStateComponent_H

#include "violet/component/component.h"
#include "violet/utility/unique_val.h"
#include "violet/handle/handle.h"

#include <string>
#include <Vector>

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UiStateComponent : public ComponentBase<UiStateComponent>
	{
	public:

		VIOLET_API friend Serializer & operator<<(Serializer & Serializer, const UiStateComponent & component);
		VIOLET_API friend Deserializer & operator>>(Deserializer & Deserializer, UiStateComponent & component);

	public:

		static Tag getStaticTag();

	public:

		UiStateComponent(Handle entityId, Deserializer & Deserializer);

		uint32 getCurrentIndex() const;
		void setCurrentIndex(uint32 index);

	private:

		uint32 m_currentIndex;
		Handle m_currentHandle;
		std::Vector<unique_val<Entity>> m_states;
	};

	VIOLET_API Serializer & operator<<(Serializer & Serializer, const UiStateComponent & component);
	VIOLET_API Deserializer & operator>>(Deserializer & Deserializer, UiStateComponent & component);
}

#endif
