#include "violet/math/math.h"
#include "violet/structures/array.h"

void BOX2_(init_point)(BOX2 *b, const V2 *p)
{
	b->min = *p;
	b->max = *p;
}

void BOX2_(from_line)(BOX2 *box, const V2 *start, const V2 *end)
{
	SCALAR xmin, xmax, ymin, ymax;

	if (start->x < end->x)
	{
		xmin = start->x;
		xmax = end->x;
	}
	else
	{
		xmin = end->x;
		xmax = start->x;
	}

	if (start->y < end->y)
	{
		ymin = start->y;
		ymax = end->y;
	}
	else
	{
		ymin = end->y;
		ymax = start->y;
	}

	BOX2_(from_dims)(box, xmin, ymax, xmax, ymin);
}

void BOX2_(from_center)(BOX2 *box, const V2 *center, const V2 *half_dim)
{
	box->min.x = center->x - half_dim->x;
	box->min.y = center->y - half_dim->x;
	box->max.x = center->x + half_dim->x;
	box->max.y = center->y + half_dim->x;
}

void BOX2_(from_dims)(BOX2 *box, SCALAR left, SCALAR top,
                      SCALAR right, SCALAR bottom)
{
	box->min.x = left;
	box->min.y = bottom;
	box->max.x = right;
	box->max.y = top;
}

b8 BOX2_(empty)(const BOX2 *box)
{
	return V2_(equal)(&box->min, &box->max);
}

b8 BOX2_(contains_point)(const BOX2 *box, const V2 *point)
{
	return point->x >= box->min.x
		&& point->x <= box->max.x
		&& point->y >= box->min.y
		&& point->y <= box->max.y;
}

b8 BOX2_(contains_box)(const BOX2 *lhs, const BOX2 *rhs)
{
	IVAL lhs_x = { lhs->min.x, lhs->max.x };
	IVAL rhs_x = { rhs->min.x, rhs->max.x };
	IVAL lhs_y = { lhs->min.y, lhs->max.y };
	IVAL rhs_y = { rhs->min.y, rhs->max.y };
	return IVAL_(contains_ival)(&lhs_x, &rhs_x)
		&& IVAL_(contains_ival)(&lhs_y, &rhs_y);
}

void BOX2_(clamp_point)(const BOX2 *b, V2 *p)
{
	p->x = MATH_(clamp)(b->min.x, p->x, b->max.x);
	p->y = MATH_(clamp)(b->min.y, p->y, b->max.y);
}

b8 BOX2_(overlaps)(const BOX2 *lhs, const BOX2 *rhs)
{
	IVAL lhs_x = { lhs->min.x, lhs->max.x };
	IVAL rhs_x = { rhs->min.x, rhs->max.x };
	IVAL lhs_y = { lhs->min.y, lhs->max.y };
	IVAL rhs_y = { rhs->min.y, rhs->max.y };
	return IVAL_(overlaps)(&lhs_x, &rhs_x)
		&& IVAL_(overlaps)(&lhs_y, &rhs_y);
}

b8 BOX2_(eq)(const BOX2 *lhs, const BOX2 *rhs)
{
	return lhs->min.x == rhs->min.x
	    && lhs->min.y == rhs->min.y
	    && lhs->max.x == rhs->max.x
	    && lhs->max.y == rhs->max.y;
}

void BOX2_(extend_point)(BOX2 *box, const V2 *point)
{
	BOX2_(from_dims)(box,
		min(box->min.x, point->x),
		max(box->max.y, point->y),
		max(box->max.x, point->x),
		min(box->min.y, point->y));
}

void BOX2_(extend_box)(BOX2 *lhs, const BOX2 *rhs)
{
	BOX2_(extend_point)(lhs, &rhs->min);
	BOX2_(extend_point)(lhs, &rhs->max);
}

void BOX2_(translate)(BOX2 *box, const V2 *off)
{
	V2_(add)(&box->min, off, &box->min);
	V2_(add)(&box->max, off, &box->max);
}

void BOX2_(transform)(BOX2 *box, const M3 mat)
{
	V2 poly[4];
	const V2 top_left = { box->min.x, box->max.y };
	const V2 bottom_right = { box->max.x, box->min.y };

	M3_(mul_v2)(mat, &top_left, poly);
	M3_(mul_v2)(mat, &box->max, poly+1);
	M3_(mul_v2)(mat, &bottom_right, poly+2);
	M3_(mul_v2)(mat, &box->min, poly+3);

	POLY_(bounding_box)(poly, 4, box);
}

void BOX2_(get_center)(const BOX2 *box, V2 *center)
{
	BOX2_(get_half_dim)(box, center);
	V2_(add)(center, &box->min, center);
}

void BOX2_(get_half_dim)(const BOX2 *box, V2 *half_dim)
{
	V2_(sub)(&box->max, &box->min, half_dim);
	V2_(scale_inv)(half_dim, 2, 2, half_dim);
}

