#ifndef VIOLET_AlignmentComponent_H
#define VIOLET_AlignmentComponent_H

#include "violet/component/component.h"
#include "violet/math/aabb.h"
#include "violet/task/thread.h"

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
		AlignmentComponent(Handle entityId, Deserializer & Deserializer);
		AlignmentComponent(AlignmentComponent && other);

	public:

		Handle m_parent;
		float m_x, m_y, m_width, m_height;
	};

	VIOLET_API Deserializer & operator>>(Deserializer & Deserializer, AlignmentComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & Serializer, const AlignmentComponent & component);
}

#endif