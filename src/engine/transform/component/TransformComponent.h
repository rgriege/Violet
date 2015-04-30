#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "engine/component/Component.h"

#include "engine/math/Vec2.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API TransformComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		TransformComponent(const Entity & entity);
		TransformComponent(const Entity & entity, Deserializer & deserializer);
		TransformComponent(const Entity & entity, Vec2f position, float rotation);
		TransformComponent(TransformComponent && other);
		TransformComponent & operator=(TransformComponent && other);

	public:

		Vec2f m_position;
		float m_rotation;
	};

	Serializer & operator<<(Serializer & serializer, const TransformComponent & component);
}

#endif