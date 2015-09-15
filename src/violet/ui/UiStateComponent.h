#ifndef VIOLET_UiStateComponent_H
#define VIOLET_UiStateComponent_H

#include "violet/component/Component.h"

#include <vector>

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API UiStateComponent : public ComponentBase<UiStateComponent>
	{
	public:

		friend Serializer & operator<<(Serializer & serializer, const UiStateComponent & component);
		friend Deserializer & operator>>(Deserializer & deserializer, UiStateComponent & component);

	public:

		static Tag getStaticTag();

	public:

		UiStateComponent(Entity & owner, Deserializer & deserializer);

		uint32 getCurrentIndex() const;
		void setCurrentIndex(uint32 index);

	private:

		uint32 m_currentIndex;
		std::vector<std::string> m_states;
	};
}

#endif