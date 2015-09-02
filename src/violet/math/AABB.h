#ifndef VIOLET_AABB_H
#define VIOLET_AABB_H

#include "violet/math/Matrix3.h"
#include "violet/math/Vec2.h"
#include "violet/math/Interval.h"

namespace Violet
{
	class AABB
	{
	public:

		static AABB createFromLine(const Vec2f & start, const Vec2f & end);

	public:

		AABB();
		AABB(const Vec2f & center, const Vec2f & halfDim);
		AABB(float left, float top, float right, float bottom);

		bool contains(const Vec2f & point) const;
		bool contains(const AABB & other) const;
		bool overlaps(const AABB & other) const;

		AABB & extend(const Vec2f & point);
		AABB & extend(const AABB & other);
		AABB & translate(const Vec2f & offset);
		AABB & transform(const Matrix3f & transformation);

		Vec2f getCenter() const;
		Vec2f getHalfDim() const;
		Vec2f getMinimum() const;
		Vec2f getMaximum() const;

	private:

		Vec2f m_topLeft;
		Vec2f m_bottomRight;
	};
}

#endif
