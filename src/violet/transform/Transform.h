#ifndef VIOLET_Transform_H
#define VIOLET_Transform_H

#include "violet/math/Matrix4.h"

namespace Violet
{
	class VIOLET_API Transform
	{
	public:

		static Vec2f getPosition(const Matrix4f & mat);
		static void setPosition(Matrix4f & mat, const Vec2f & position);

	private:

		Transform() = delete;
	};
}

#endif