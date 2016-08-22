/*
 * Translated from Mark Bayazit's C++ decomposition algorithm.
 * http://mnbayazit.com/406/bayazit
 */

#include <assert.h>
#include <string.h>

#include "violet/structures/array.h"
#include "violet/math/decompose.h"
#include "violet/math/r32.h"

static
r32 _area(const v2f *a, const v2f *b, const v2f *c)
{
	return (((b->x-a->x)*(c->y-a->y))-((c->x-a->x)*(b->y-a->y)));
}

static
b8 _left(const v2f *a, const v2f *b, const v2f *c)
{
	return _area(a, b, c) > 0;
}

static
b8 _left_on(const v2f *a, const v2f *b, const v2f *c)
{
	return _area(a, b, c) >= 0;
}

static
b8 _right(const v2f *a, const v2f *b, const v2f *c)
{
	return _area(a, b, c) < 0;
}

static
b8 _right_on(const v2f *a, const v2f *b, const v2f *c)
{
	return _area(a, b, c) <= 0;
}

static
b8 _is_reflex(const v2f *a, const v2f *b, const v2f *c)
{
	return _right(a, b, c);
}

void polyf_decompose(const v2f *v, u32 n, array *polys)
{
	assert(polyf_is_cc(v, n));

	v2f upper_int, lower_int, p;
	r32 upper_dist, lower_dist, d, closest_dist;
	u32 upper_idx, lower_idx, closest_idx;
	array lower_poly, upper_poly;

	array_init(&lower_poly, sizeof(v2f));
	array_init(&upper_poly, sizeof(v2f));

	const v2f *vi0 = v+n-1;
	for (u32 i = 0; i < n; ++i)
	{
		const v2f *vi1 = v+i;
		const v2f *vi2 = v+(i+1)%n;
		if (!_is_reflex(vi0, vi1, vi2))
			goto inxt;

		upper_dist = lower_dist =  FLT_MAX;
		const v2f *vj0 = v+n-1;
		for (u32 j = 0; j < n; ++j)
		{
			const v2f *vj1 = v+j;
			const v2f *vj2 = v+(j+1)%n;
			if (   _left(vi0, vi1, vj1)
			    && _right_on(vi0, vi1, vj0))
			{
				assert(mathf_line_intersect(vi0, vi1, vj0, vj1, &p));
				if (_right(vi2, vi1, &p))
				{
					d = v2f_dist_sq(vi1, &p);
					if (d < lower_dist)
					{
						lower_dist = d;
						lower_int = p;
						lower_idx = j;
					}
				}
			}
			if (   _left(vi2, vi1, vj2)
			    && _right_on(vi2, vi1, vj1))
			{
				assert(mathf_line_intersect(vi2, vi1, vj1, vj2, &p));
				if (_left(vi0, vi1, &p))
				{
					d = v2f_dist_sq(vi1, &p);
					if (d < upper_dist)
					{
						upper_dist = d;
						upper_int = p;
						upper_idx = j;
					}
				}
			}
			vj0 = vj1;
		}

		if (lower_idx == (upper_idx+1)%n)
		{
			// case 1: no vertices to connect to, choose a point in the middle
			p.x = (lower_int.x+upper_int.x)/2;
			p.y = (lower_int.y+upper_int.y)/2;

			if (i < upper_idx)
			{
				for (u32 j = i; j <= upper_idx; ++j)
					array_append(&lower_poly, v+j);
				array_append(&lower_poly, &p);
				array_append(&upper_poly, &p);
				if (lower_idx != 0)
				{
					for (u32 j = lower_idx; j < n; ++j)
						array_append(&upper_poly, v+j);
				}
				for (u32 j = 0; j <= i; ++j)
					array_append(&upper_poly, v+j);
			}
			else
			{
				if (i != 0)
				{
					for (u32 j = i; j < n; ++j)
						array_append(&lower_poly, v+j);
				}
				for (u32 j = 0; j <= upper_idx; ++j)
					array_append(&lower_poly, v+j);
				array_append(&lower_poly, &p);
				array_append(&upper_poly, &p);
				for (u32 j = lower_idx; j <= i; ++j)
					array_append(&upper_poly, v+j);
			}
		}
		else
		{
			// case 2: connect to closest point within the triangle
			if (lower_idx > upper_idx)
				upper_idx += n;
			closest_dist = FLT_MAX;

			for (u32 j = lower_idx; j <= upper_idx; ++j)
			{
				const v2f *vj = j < 0 ? v+j%n+n : v+j%n;
				if (   _left_on(vi0, vi1, vj)
				    && _right_on(vi2, vi1, vj))
				{
					d = v2f_dist_sq(vi1, vj);
					if (d < closest_dist)
					{
						closest_dist = d;
						closest_idx = j % n;
					}
				}

			}

			if (i < closest_idx)
			{
				for (u32 j = i; j <= closest_idx; ++j)
					array_append(&lower_poly, v+j);
				if (closest_idx != 0)
				{
					for (u32 j = closest_idx; j < n; ++j)
						array_append(&upper_poly, v+j);
				}
				for (u32 j = 0; j <= i; ++j)
					array_append(&upper_poly, v+j);
			}
			else
			{
				if (i != 0)
				{
					for (u32 j = i; j < n; ++j)
						array_append(&lower_poly, v+j);
				}
				for (u32 j = 0; j <= closest_idx; ++j)
					array_append(&lower_poly, v+j);
				for (u32 j = closest_idx; j <= i; ++j)
					array_append(&upper_poly, v+j);
			}
		}

		// solve smallest poly first
		if (lower_poly.size < upper_poly.size)
		{
			polyf_decompose(lower_poly.data, lower_poly.size, polys);
			polyf_decompose(upper_poly.data, upper_poly.size, polys);
		}
		else
		{
			polyf_decompose(upper_poly.data, upper_poly.size, polys);
			polyf_decompose(lower_poly.data, lower_poly.size, polys);
		}
		return;

inxt:
		vi0 = vi1;
	}

	array_destroy(&lower_poly);
	array_destroy(&upper_poly);

	array *poly = array_append_null(polys);
	array_init(poly, sizeof(v2f));
	array_reserve(poly, n);
	poly->size = n;
	memcpy(poly->data, v, n*sizeof(v2f));
}
