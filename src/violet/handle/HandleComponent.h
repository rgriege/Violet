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

		friend class Scene;

		VIOLET_API friend Deserializer & operator>>(Deserializer & deserializer, HandleComponent & component);
		VIOLET_API friend Serializer & operator<<(Serializer & serializer, const HandleComponent & component);

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

	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, HandleComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const HandleComponent & component);
}

#endif
