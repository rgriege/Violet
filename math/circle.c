#include "violet/math/circle.h"

interval circle_project(const circle * c, const v2 * axis)
{
	v2 unit_axis = *axis;
	if (!v2_is_unit(&unit_axis))
		v2_normalize(&unit_axis, &unit_axis);

	r32 center_proj = v2_dot(&c->center, &unit_axis);
	interval result = { center_proj - c->radius, center_proj + c->radius };
	return result;
}

