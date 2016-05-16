#include "violet/math/ibox.h"
#include "violet/math/ival.h"
#include "violet/math/math.h"
#include "violet/math/poly.h"

void ibox_from_line(ibox * box, const v2i * start, const v2i * end)
{
	s32 xmin, xmax, ymin, ymax;

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

	ibox_from_dims(box, xmin, ymax, xmax, ymin);
}

void ibox_from_center(ibox * box, const v2i * center, const v2i * half_dim)
{
	v2i_sub(center, half_dim, &box->top_left);
	v2i_add(center, half_dim, &box->bottom_right);
}

void ibox_from_dims(ibox * box, s32 left, s32 top, s32 right, s32 bottom)
{
	box->top_left.x = left;
	box->top_left.y = top;
	box->bottom_right.x = right;
	box->bottom_right.y = bottom;
}

b8 ibox_empty(const ibox * box)
{
	return v2i_equal(&box->top_left, &box->bottom_right);
}

b8 ibox_contains_point(const ibox * box, const v2i * point)
{
	return point->x >= box->top_left.x
		&& point->x <= box->bottom_right.x
		&& point->y <= box->top_left.y
		&& point->y >= box->bottom_right.y;
}

b8 ibox_contains_ibox(const ibox * lhs, const ibox * rhs)
{
	ival lhs_x = { lhs->top_left.x, lhs->bottom_right.x };
	ival rhs_x = { rhs->top_left.x, rhs->bottom_right.x };
	ival lhs_y = { lhs->top_left.y, lhs->bottom_right.y };
	ival rhs_y = { rhs->top_left.y, rhs->bottom_right.y };
	return ival_contains_ival(&lhs_x, &rhs_x) && ival_contains_ival(&lhs_y, &rhs_y);
}

b8 ibox_overlaps(const ibox * lhs, const ibox * rhs)
{
	ival lhs_x = { lhs->top_left.x, lhs->bottom_right.x };
	ival rhs_x = { rhs->top_left.x, rhs->bottom_right.x };
	ival lhs_y = { lhs->top_left.y, lhs->bottom_right.y };
	ival rhs_y = { rhs->top_left.y, rhs->bottom_right.y };
	return ival_overlaps(&lhs_x, &rhs_x) && ival_overlaps(&lhs_y, &rhs_y);
}

void ibox_extend_point(ibox * box, const v2i * point)
{
	ibox_from_dims(box,
		min(box->top_left.x, point->x),
		max(box->top_left.y, point->y),
		max(box->bottom_right.x, point->x),
		min(box->bottom_right.y, point->y));
}

void ibox_extend_ibox(ibox * lhs, const ibox * rhs)
{
	ibox_extend_point(lhs, &rhs->top_left);
	ibox_extend_point(lhs, &rhs->bottom_right);
}

void ibox_translate(ibox * box, const v2i * off)
{
	v2i_add(&box->top_left, off, &box->top_left);
	v2i_add(&box->bottom_right, off, &box->bottom_right);
}

void ibox_get_center(const ibox * box, v2i * center)
{
	center->x = (box->bottom_right.x - box->top_left.x) / 2;
	center->y = (box->top_left.y - box->bottom_right.y) / 2;
}

void ibox_get_half_dim(const ibox * box, v2i * half_dim)
{
	v2i center;
	ibox_get_center(box, &center);
	v2i_sub(&center, &box->top_left, half_dim);
}

void ibox_get_min(const ibox * box, v2i * min)
{
	min->x = box->top_left.x;
	min->y = box->bottom_right.y;
}

void ibox_get_max(const ibox * box, v2i * max)
{
	max->x = box->bottom_right.x;
	max->y = box->top_left.y;
}

