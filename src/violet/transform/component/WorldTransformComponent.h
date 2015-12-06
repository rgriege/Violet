#ifndef VIOLET_WorldTransformComponent_H
#define VIOLET_WorldTransformComponent_H

#include "violet/component/Component.h"
#include "violet/task/Thread.h"

#include "violet/math/Matrix3.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API WorldTransformComponent : public ComponentBase<WorldTransformComponent>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		WorldTransformComponent(Handle entityId);
		WorldTransformComponent(Handle entityId, Deserializer & deserializer);
		WorldTransformComponent(Handle entityId, const Matrix3f & transform);
		WorldTransformComponent(WorldTransformComponent && other);

	public:

		Matrix3f m_transform;
	};

	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, WorldTransformComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const WorldTransformComponent & component);
}

#endif