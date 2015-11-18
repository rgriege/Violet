#ifndef VIOLET_HandleComponent_H
#define VIOLET_HandleComponent_H

#include "violet/component/Component.h"

#include "violet/handle/Handle.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API HandleComponent : public ComponentBase<HandleComponent>
	{
	public:

		friend Deserializer & operator>>(Deserializer & deserializer, HandleComponent & component);
		friend Serializer & operator<<(Serializer & serializer, const HandleComponent & component);

	public:

		static Tag getStaticTag();

	public:

		HandleComponent(Entity & owner);
		HandleComponent(Entity & owner, Deserializer & deserializer);
		HandleComponent(HandleComponent && other);
		~HandleComponent();

		Handle getHandle() const;

	private:

		HandleComponent(HandleComponent const & rhs) = delete;
		HandleComponent & operator=(HandleComponent const & rhs) = delete;

	private:

		Handle m_handle;
	};

    Deserializer & operator>>(Deserializer & deserializer, HandleComponent & component);
    Serializer & operator<<(Serializer & serializer, const HandleComponent & component);
}

#endif
