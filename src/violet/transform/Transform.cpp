// ============================================================================

#include "violet/transform/Transform.h"

using namespace Violet;

// ============================================================================

const Vec2f Transform::NoScale(1, 1);

// ============================================================================

Vec2f Transform::getPosition(const Matrix3f & mat)
{
	return Vec2f(mat[0][2], mat[1][2]);
}

// ----------------------------------------------------------------------------

void Transform::setPosition(Matrix3f & mat, const Vec2f & position)
{
	mat[0][2] = position.x;
	mat[1][2] = position.y;
}

// ----------------------------------------------------------------------------

Matrix3f Transform::create(const Vec2f & position, const float rotation, const Vec2f & scale)
{
	const Matrix3f result = {
		scale.x * cos(rotation), -scale.y * sin(rotation), position.x,
		scale.x * sin(rotation), scale.y * cos(rotation), position.y,
		0, 0, 1
	};
	return result;
}

// ============================================================================
