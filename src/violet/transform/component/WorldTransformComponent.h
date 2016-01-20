#ifndef VIOLET_WorldTransformComponent_H
#define VIOLET_WorldTransformComponent_H

#include "violet/component/Component.h"
#include "violet/task/Thread.h"

#include "violet/math/Matrix4.h"

namespace Violet
{
	class ComponentDeserializer;
	class Serializer;

	class VIOLET_API WorldTransformComponent : public ComponentBase<WorldTransformComponent, 1>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		WorldTransformComponent(EntityId entityId);
		WorldTransformComponent(EntityId entityId, ComponentDeserializer & deserializer);
		WorldTransformComponent(EntityId entityId, const Matrix4f & transform);

	public:

		Matrix4f m_transform;
	};

	VIOLET_API ComponentDeserializer & operator>>(ComponentDeserializer & deserializer, WorldTransformComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const WorldTransformComponent & component);
}

#endif
