#include "violet/math/math.h"
#include "violet/structures/array.h"

void BOX2_(init_point)(BOX2 *b, const V2 *p)
{
	b->top_left = *p;
	b->bottom_right = *p;
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
	V2_(sub)(center, half_dim, &box->top_left);
	V2_(add)(center, half_dim, &box->bottom_right);
}

void BOX2_(from_dims)(BOX2 *box, SCALAR left, SCALAR top,
                         SCALAR right, SCALAR bottom)
{
	box->top_left.x = left;
	box->top_left.y = top;
	box->bottom_right.x = right;
	box->bottom_right.y = bottom;
}

b8 BOX2_(empty)(const BOX2 *box)
{
	return V2_(equal)(&box->top_left, &box->bottom_right);
}

b8 BOX2_(contains_point)(const BOX2 *box, const V2 *point)
{
	return point->x >= box->top_left.x
		&& point->x <= box->bottom_right.x
		&& point->y <= box->top_left.y
		&& point->y >= box->bottom_right.y;
}

b8 BOX2_(contains_box)(const BOX2 *lhs, const BOX2 *rhs)
{
	IVAL lhs_x = { lhs->top_left.x, lhs->bottom_right.x };
	IVAL rhs_x = { rhs->top_left.x, rhs->bottom_right.x };
	IVAL lhs_y = { lhs->top_left.y, lhs->bottom_right.y };
	IVAL rhs_y = { rhs->top_left.y, rhs->bottom_right.y };
	return IVAL_(contains_ival)(&lhs_x, &rhs_x) && IVAL_(contains_ival)(&lhs_y, &rhs_y);
}

b8 BOX2_(overlaps)(const BOX2 *lhs, const BOX2 *rhs)
{
	IVAL lhs_x = { lhs->top_left.x, lhs->bottom_right.x };
	IVAL rhs_x = { rhs->top_left.x, rhs->bottom_right.x };
	IVAL lhs_y = { lhs->top_left.y, lhs->bottom_right.y };
	IVAL rhs_y = { rhs->top_left.y, rhs->bottom_right.y };
	return IVAL_(overlaps)(&lhs_x, &rhs_x) && IVAL_(overlaps)(&lhs_y, &rhs_y);
}

void BOX2_(extend_point)(BOX2 *box, const V2 *point)
{
	BOX2_(from_dims)(box,
		min(box->top_left.x, point->x),
		max(box->top_left.y, point->y),
		max(box->bottom_right.x, point->x),
		min(box->bottom_right.y, point->y));
}

void BOX2_(extend_box)(BOX2 *lhs, const BOX2 *rhs)
{
	BOX2_(extend_point)(lhs, &rhs->top_left);
	BOX2_(extend_point)(lhs, &rhs->bottom_right);
}

void BOX2_(translate)(BOX2 *box, const V2 *off)
{
	V2_(add)(&box->top_left, off, &box->top_left);
	V2_(add)(&box->bottom_right, off, &box->bottom_right);
}

void BOX2_(transform)(BOX2 *box, const M3 mat)
{
	V2 poly[4];
	const V2 top_right = { box->bottom_right.x, box->top_left.y };
	const V2 bottom_left = { box->top_left.x, box->bottom_right.y };

	M3_(mul_v2)(mat, &box->top_left, poly);
	M3_(mul_v2)(mat, &top_right, poly+1);
	M3_(mul_v2)(mat, &box->bottom_right, poly+2);
	M3_(mul_v2)(mat, &bottom_left, poly+3);

	POLY_(bounding_box)(poly, 4, box);
}

void BOX2_(get_center)(const BOX2 *box, V2 *center)
{
	center->x = (box->bottom_right.x - box->top_left.x) / 2;
	center->y = (box->top_left.y - box->bottom_right.y) / 2;
}

void BOX2_(get_half_dim)(const BOX2 *box, V2 *half_dim)
{
	V2 center;
	BOX2_(get_center)(box, &center);
	V2_(sub)(&center, &box->top_left, half_dim);
}

void BOX2_(get_min)(const BOX2 *box, V2 *min)
{
	min->x = box->top_left.x;
	min->y = box->bottom_right.y;
}

void BOX2_(get_max)(const BOX2 *box, V2 *max)
{
	max->x = box->bottom_right.x;
	max->y = box->top_left.y;
}

