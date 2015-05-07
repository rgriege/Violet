#ifndef VIOLET_TransformComponent_H
#define VIOLET_TransformComponent_H

#include "engine/component/Component.h"

#include "engine/math/Vec2.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API TransformComponent : public Component<TransformComponent>
	{
	public:

		static Tag getTypeId();

	public:

		TransformComponent(Entity entity);
		TransformComponent(Entity entity, Deserializer & deserializer);
		TransformComponent(Entity entity, Vec2f position, float rotation);
		TransformComponent(TransformComponent && other);
		TransformComponent & operator=(TransformComponent && other);

	public:

		Vec2f m_position;
		float m_rotation;
	};

	Deserializer & operator>>(Deserializer & deserializer, TransformComponent & component);
	Serializer & operator<<(Serializer & serializer, const TransformComponent & component);
}

#endif