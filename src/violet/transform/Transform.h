#ifndef VIOLET_Transform_H
#define VIOLET_Transform_H

#include "violet/math/Matrix3.h"

namespace Violet
{
	class VIOLET_API Transform
	{
	public:

		static Vec2f getPosition(const Matrix3f & mat);
		static void setPosition(Matrix3f & mat, const Vec2f & position);

		// applied scale -> rotation -> translation
		static Matrix3f create(const Vec2f & position, float rotation = 0.f, const Vec2f & scale = NoScale);

	public:

		static const Vec2f NoScale;

	private:

		Transform();
	};
}

#endif