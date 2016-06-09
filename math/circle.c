#include "violet/math/circle.h"

interval circle_project(const circle * c, const v2f * axis)
{
	v2f unit_axis = *axis;
	if (!v2f_is_unit(&unit_axis))
		v2f_normalize(&unit_axis, &unit_axis);

	r32 center_proj = v2f_dot(&c->center, &unit_axis);
	interval result = { center_proj - c->radius, center_proj + c->radius };
	return result;
}

