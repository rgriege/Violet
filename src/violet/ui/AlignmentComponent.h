#ifndef VIOLET_AlignmentComponent_H
#define VIOLET_AlignmentComponent_H

#include "violet/component/Component.h"
#include "violet/math/AABB.h"
#include "violet/task/Thread.h"

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API AlignmentComponent : public ComponentBase<AlignmentComponent>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		AlignmentComponent(Handle entityId);
		AlignmentComponent(Handle entityId, Deserializer & deserializer);
		AlignmentComponent(AlignmentComponent && other);

	public:

		Handle m_parent;
		float m_x, m_y, m_width, m_height;
	};

	VIOLET_API Deserializer & operator>>(Deserializer & deserializer, AlignmentComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const AlignmentComponent & component);
}

#endif