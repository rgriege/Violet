// ============================================================================

#include "violet/transform/Transform.h"

using namespace Violet;

// ============================================================================

Vec2f Transform::getPosition(const Matrix4f & mat)
{
	return Vec2f(mat[0][3], mat[1][3]);
}

// ----------------------------------------------------------------------------

void Transform::setPosition(Matrix4f & mat, const Vec2f & position)
{
	mat[0][3] = position.x;
	mat[1][3] = position.y;
}

// ============================================================================
