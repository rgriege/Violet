#ifndef VIOLET_TransformComponent_H
#define VIOLET_TransformComponent_H

#include "violet/component/Component.h"

#include "violet/math/Matrix3.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API TransformComponent : public ComponentBase<TransformComponent>
	{
	public:

		static Tag getStaticTag();

	public:

		TransformComponent(const Entity & owner);
		TransformComponent(const Entity & owner, Deserializer & deserializer);
		TransformComponent(const Entity & owner, const Matrix3f & transform);
		TransformComponent(TransformComponent && other);

	public:

		Matrix3f m_transform;
	};

	Deserializer & operator>>(Deserializer & deserializer, TransformComponent & component);
	Serializer & operator<<(Serializer & serializer, const TransformComponent & component);
}

#endif