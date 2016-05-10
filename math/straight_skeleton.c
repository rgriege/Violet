#include <assert.h>
#include <float.h>
#include <limits.h>
#include <string.h>

#include "violet/math/line.h"
#include "violet/math/straight_skeleton.h"
#include "violet/structures/pqueue.h"

typedef struct _ss_edge
{
	v2 start;
	v2 end;
} _ss_edge;

typedef struct _ss_active_vtx
{
	u32 src_idx;
	_ss_edge edges[2];
	v2 bisector;
	b8 reflex;
} _ss_active_vtx;

static void _compute_bisector(const v2 * a0, const v2 * a1,
                              const v2 * b0, const v2 * b1, v2 * bisector)
{
	v2 a, b;
	v2_sub(a1, a0, &a);
	v2_normalize(&a, &a);

	v2_sub(b1, b0, &b);
	v2_normalize(&b, &b);

	v2_add(&a, &b, bisector);
	v2_normalize(bisector, bisector);
}

static void _ss_active_vtx_compute_bisector(_ss_active_vtx * vtx)
{
	_compute_bisector(&vtx->edges[0].start, &vtx->edges[0].end,
		&vtx->edges[1].start, &vtx->edges[1].end, &vtx->bisector);
}


typedef struct _ss_isec
{
	r32 t;
	v2 p;
	u32 src_idcs[2];
	u32 split_src_idx;
	u32 vertex_list_idx;
} _ss_isec;

static b8 _ss_isec_less(void * lhs, void * rhs)
{
	return ((_ss_isec*)lhs)->t < ((_ss_isec*)rhs)->t;
}


const v2 * ss_vertex(straight_skeleton * s, u32 idx)
{
	const u32 src_sz = array_size(&s->src->vertices);
	assert(idx < src_sz + array_size(&s->spine_vertices));
	return (idx < src_sz) ?
		array_get(&s->src->vertices, idx) :
		array_get(&s->spine_vertices, idx - src_sz);
}

static void _add_intersection(straight_skeleton * s, const _ss_active_vtx * a,
                              const _ss_active_vtx * b, pqueue * isecs,
                              u32 vertex_list_idx)
{
	r32 t, u;
	if (ray_intersect_coords(ss_vertex(s, a->src_idx), &a->bisector,
		ss_vertex(s, b->src_idx), &b->bisector, &t, &u) && t > 0 && u > 0)
	{
		_ss_isec isec;
		v2_scale(&a->bisector, t, t, &isec.p);
		v2_add(&isec.p, ss_vertex(s, a->src_idx), &isec.p);

		isec.src_idcs[0] = a->src_idx;
		isec.src_idcs[1] = b->src_idx;
		isec.t = point_to_line_dist(&b->edges[1].start, &b->edges[1].end,
			&isec.p);
		isec.split_src_idx = UINT_MAX;
		isec.vertex_list_idx = vertex_list_idx;
		pqueue_add(isecs, &isec);
	}
}

static void _add_split_intersection(array * vertices, straight_skeleton * s,
                                    const _ss_active_vtx * vtx, pqueue * isecs,
                                    u32 vertex_list_idx, b8 cc)
{
	const v2 * vtx_p = ss_vertex(s, vtx->src_idx);
	_ss_isec isec = { .t = FLT_MAX, .split_src_idx = vtx->src_idx,
		.vertex_list_idx = vertex_list_idx };

	for (u32 i = 0, n = array_size(vertices); i < n; ++i)
	{
		const v2 * e0 = array_get(vertices, i);
		const u32 inext = i < n - 1 ? i + 1 : 0;
		const v2 * e1 = array_get(vertices, inext);

		r32 t0, u0, t1, u1;
		if (   line_intersect_coords(&vtx->edges[0].end, &vtx->edges[0].start, e0, e1, &t0, &u0)
			&& t0 > 1
			&& line_intersect_coords(&vtx->edges[1].end, &vtx->edges[1].start, e0, e1, &t1, &u1)
			&& t1 > 1)
		{
			v2 a, b;
			line_extrapolate(&vtx->edges[0].end, &vtx->edges[0].start, t0, &a);
			line_extrapolate(&vtx->edges[1].end, &vtx->edges[1].start, t1, &b);
			r32 a_len = v2_dist(&b, vtx_p);
			r32 b_len = v2_dist(&a, vtx_p);
			r32 c_len = v2_dist(&a, &b);
			v2 a_contrib, b_contrib, c_contrib, incenter = { 0, 0 };
			v2_scale(&a, a_len, a_len, &a_contrib);
			v2_scale(&b, b_len, b_len, &b_contrib);
			v2_scale(vtx_p, c_len, c_len, &c_contrib);
			v2_add(&incenter, &a_contrib, &incenter);
			v2_add(&incenter, &b_contrib, &incenter);
			v2_add(&incenter, &c_contrib, &incenter);
			const r32 denom = 1.f / (a_len + b_len + c_len);
			v2_scale(&incenter, denom, denom, &incenter);

			const u32 iprev = i > 0 ? i - 1 : n - 1;
			const v2 * e0_prev = array_get(vertices, iprev);
			const u32 inextnext = inext < n - 1 ? inext + 1 : 0;
			const v2 * e1_next = array_get(vertices, inextnext);
			v2 e, e_prev, e_next;
			v2_sub(e1, e0, &e);
			v2_sub(e0, e0_prev, &e_prev);
			v2_sub(e1_next, e1, &e_next);

			v2 e0_bisector, e1_bisector;
			_compute_bisector(e0, e0_prev, e0, e1, &e0_bisector);
			if (cc != (v2_cross(&e_prev, &e) > 0.f))
				v2_scale(&e0_bisector, -1, -1, &e0_bisector);
			_compute_bisector(e1, e0, e1, e1_next, &e1_bisector);
			if (cc != (v2_cross(&e, &e_next) > 0.f))
				v2_scale(&e1_bisector, -1, -1, &e1_bisector);

			v2 e0_to_incenter, e1_to_incenter;
			v2_sub(&incenter, e0, &e0_to_incenter);
			v2_sub(&incenter, e1, &e1_to_incenter);

			const r32 x = v2_cross(&e0_bisector, &e0_to_incenter);
			const r32 y = v2_cross(&e, &e0_to_incenter);
			const r32 z = v2_cross(&e1_bisector, &e1_to_incenter);
			if (x < 0.f && y > 0.f && z > 0.f)
			{
				const r32 isec_t = point_to_line_dist(e0, e1, &incenter);
				if (isec_t < isec.t)
				{
					isec.t = isec_t;
					isec.p = incenter;
					isec.src_idcs[0] = i;
					isec.src_idcs[1] = inext;
				}
			}
		}
	}
	if (isec.t < FLT_MAX)
		pqueue_add(isecs, &isec);
}

static void _add_intersections(poly * p, straight_skeleton * s, ss_debug * dbg)
{
	for (u32 i = 0, n = dbg->active_vertex_lists.size; i < n; ++i)
	{
		array * active_vertices = array_get(&dbg->active_vertex_lists, i);
		for (u32 j = 0, jn = active_vertices->size; j < jn; ++j)
		{
			const _ss_active_vtx * a = array_get(active_vertices, j);
			const _ss_active_vtx * b = array_get(active_vertices, j < jn - 1 ? j + 1 : 0);

			_add_intersection(s, a, b, &dbg->isecs, i);
			if (a->reflex)
				_add_split_intersection(&p->vertices, s, a, &dbg->isecs, i, dbg->cc);
		}
	}
}

static void _add_edge(straight_skeleton * s, u32 src, u32 dst)
{
	ss_edge_pair * e = array_map_get(&s->edges, &src);
	if (!e)
	{
		e = array_map_insert_null(&s->edges, &src);
		e->dst_idx0 = dst;
		e->dst_idx1 = UINT_MAX;
	}
	else
	{
		assert(e->dst_idx1 == UINT_MAX);
		e->dst_idx1 = dst;
	}
}

b8 poly_ss_debug(poly * p, straight_skeleton * s, ss_debug * dbg)
{
	assert(array_size(&p->vertices) >= 3);

	if (!dbg->init)
	{
		s->src = p;
		array_init(&s->spine_vertices, sizeof(v2));
		array_map_init(&s->edges, sizeof(u32), sizeof(ss_edge_pair));

		dbg->cc = poly_is_cc(p);

		// gather initial active vertices
		array_init(&dbg->active_vertex_lists, sizeof(array));
		array * active_vertices = array_append_null(&dbg->active_vertex_lists);
		array_init(active_vertices, sizeof(_ss_active_vtx));
		for (u32 i = 0, n = array_size(&p->vertices); i < n; ++i)
		{
			const v2 * a = array_get(&p->vertices, i > 0 ? i - 1 : n - 1);
			const v2 * b = array_get(&p->vertices, i);
			const v2 * c = array_get(&p->vertices, i < n - 1 ? i + 1 : 0);

			_ss_active_vtx * active_vtx = array_append_null(active_vertices);
			active_vtx->src_idx = i;

			active_vtx->edges[0].start = *b;
			active_vtx->edges[0].end = *a;
			active_vtx->edges[1].start = *b;
			active_vtx->edges[1].end = *c;

			_ss_active_vtx_compute_bisector(active_vtx);

			v2 ab, bc;
			v2_sub(b, a, &ab);
			v2_sub(c, b, &bc);
			if (dbg->cc != (v2_cross(&ab, &bc) > 0.f))
			{
				v2_scale(&active_vtx->bisector, -1, -1, &active_vtx->bisector);
				active_vtx->reflex = true;
			}
			else
				active_vtx->reflex = false;
		}

		// gather initial intersections
		pqueue_init(&dbg->isecs, sizeof(_ss_isec), _ss_isec_less);
		_add_intersections(p, s, dbg);
		dbg->init = true;
		return false;
	}

	if (!pqueue_empty(&dbg->isecs))
	{
		_ss_isec isec;
		memcpy(&isec, pqueue_next(&dbg->isecs), sizeof(_ss_isec));
		pqueue_pop(&dbg->isecs);

		const u32 dst_idx = array_size(&s->src->vertices) +
			array_size(&s->spine_vertices);
		array_append(&s->spine_vertices, &isec.p);
		if (isec.split_src_idx == UINT_MAX)
		{
			// remove invalidated intersections
			for (u32 i = 0, n = array_size(&dbg->isecs.data); i < n; )
			{
				_ss_isec * other = array_get(&dbg->isecs.data, i);
				if (   isec.src_idcs[0] == other->src_idcs[0]
					|| isec.src_idcs[0] == other->src_idcs[1]
					|| isec.src_idcs[1] == other->src_idcs[0]
					|| isec.src_idcs[1] == other->src_idcs[1])
				{
					array_remove(&dbg->isecs.data, i);
					--n;
				}
				else
					++i;
			}

			_add_edge(s, isec.src_idcs[0], dst_idx);
			_add_edge(s, isec.src_idcs[1], dst_idx);

			// replace active vertices with new active vertex
			u32 new_idx = UINT_MAX;
			array * active_vertices =
				array_get(&dbg->active_vertex_lists, isec.vertex_list_idx);
			for (u32 i = 0, n = array_size(active_vertices); i < n; ++i)
			{
				_ss_active_vtx * vtx0 = array_get(active_vertices, i);
				if (vtx0->src_idx == isec.src_idcs[0])
				{
					u32 remove_idx;
					_ss_active_vtx *new_vtx, *vtx1;
					if (i < n - 1)
					{
						vtx1 = array_get(active_vertices, i + 1);
						new_idx = i;
						new_vtx = vtx0;
						remove_idx = i + 1;
					}
					else
					{
						vtx1 = array_get(active_vertices, 0);
						new_idx = 0;
						new_vtx = vtx1;
						remove_idx = i;
					}
					new_vtx->src_idx = dst_idx;
					new_vtx->edges[0] = vtx0->edges[0];
					new_vtx->edges[1] = vtx1->edges[1];
					new_vtx->reflex = false; // TODO(rgriege): always false?
					_ss_active_vtx_compute_bisector(new_vtx);
					array_remove(active_vertices, remove_idx);
					break;
				}
			}

			// add new intersections for newly active vertex
			const u32 n = array_size(active_vertices);
			assert(new_idx < n);
			if (n > 2)
			{
				const _ss_active_vtx * a = array_get(active_vertices, new_idx > 0 ? new_idx - 1 : n - 1);
				const _ss_active_vtx * b = array_get(active_vertices, new_idx);
				const _ss_active_vtx * c = array_get(active_vertices, new_idx < n - 1 ? new_idx + 1 : 0);
				_add_intersection(s, a, b, &dbg->isecs, isec.vertex_list_idx);
				_add_intersection(s, b, c, &dbg->isecs, isec.vertex_list_idx);
				if (b->reflex)
					_add_split_intersection(&p->vertices, s, b, &dbg->isecs, isec.vertex_list_idx, dbg->cc);
			}
			else
			{
				const _ss_active_vtx * a = array_get(active_vertices, 0);
				const _ss_active_vtx * b = array_get(active_vertices, 1);
				// TODO(rgriege): is this correct for edge direction?
				if (a->src_idx < b->src_idx)
					_add_edge(s, a->src_idx, b->src_idx);
				else
					_add_edge(s, b->src_idx, a->src_idx);
				array_clear(active_vertices);
			}
		}
		else
		{
			_add_edge(s, isec.split_src_idx, dst_idx);

			// split vertex list
			array * new_vtx_list = array_append_null(&dbg->active_vertex_lists);
			array_init(new_vtx_list, sizeof(_ss_active_vtx));

			array * vtx_list =
				array_get(&dbg->active_vertex_lists, isec.vertex_list_idx);

			// TODO(rgriege): handle the case where the intersection's source
			// indices could be no longer active
			u32 split_idx = UINT_MAX, start0_idx = UINT_MAX, end1_idx = UINT_MAX;
			for (u32 i = 0, n = array_size(vtx_list); i < n; ++i)
			{
				_ss_active_vtx * vtx = array_get(vtx_list, i);
				if (vtx->src_idx == isec.split_src_idx)
					split_idx = i;
				else if (vtx->src_idx == isec.src_idcs[1])
					start0_idx = i;
				else if (vtx->src_idx == isec.src_idcs[0])
					end1_idx = i;
			}
			assert(split_idx != UINT_MAX);
			assert(start0_idx != UINT_MAX);
			assert(end1_idx != UINT_MAX);

			_ss_active_vtx * end1_vtx = array_get(vtx_list, end1_idx);
			for (u32 i = split_idx;
			     i != start0_idx;
			     i = (i + 1) % vtx_list->size)
				array_append(new_vtx_list, array_get(vtx_list, i));
			_ss_active_vtx * split1_vtx = array_get(new_vtx_list, 0);
			split1_vtx->src_idx = dst_idx;
			split1_vtx->edges[0].start = end1_vtx->edges[1].end;
			split1_vtx->edges[0].end = end1_vtx->edges[1].start;
			split1_vtx->reflex = false; // TODO(rgriege): always false?
			_ss_active_vtx_compute_bisector(split1_vtx);

			_ss_active_vtx * split0_vtx = array_get(vtx_list, split_idx);
			split0_vtx->src_idx = dst_idx;
			split0_vtx->edges[1] = end1_vtx->edges[1];
			split0_vtx->reflex = false; // TODO(rgriege): always false?
			_ss_active_vtx_compute_bisector(split0_vtx);

			// remove ranges from vtx_list
			const u32 remove_idx = split_idx + 1;
			if (remove_idx < end1_idx)
			{
				for (u32 i = remove_idx; i <= end1_idx; ++i)
					array_remove(vtx_list, remove_idx < vtx_list->size ? remove_idx : 0);
			}
			else
			{
				const u32 orig_size = vtx_list->size;
				for (u32 i = 0; i < start0_idx; ++i)
					array_remove(vtx_list, 0);
				for (u32 i = remove_idx; i < orig_size; ++i)
					array_remove(vtx_list, vtx_list->size - 1);
			}

			// recalculate all intersections (easier than adjusting vertex_list indices)
			array_clear(&dbg->isecs.data);
			_add_intersections(p, s, dbg);
		}

		return false;
	}
	else
	{
		pqueue_destroy(&dbg->isecs);
		array_destroy_each(&dbg->active_vertex_lists, (void(*)(void*))array_destroy);
		dbg->init = false;
		return true;
	}
}
void straight_skeleton_destroy(straight_skeleton * s)
{
	array_destroy(&s->spine_vertices);
	array_map_destroy(&s->edges);
}

/*void polygon_inset(straight_skeleton * s, r32 inset, poly * out)
{
	for (u32 i = 0, n = s->src->vertices.size; i < n; ++i)
	{
		const ss_edge_pair * dst = array_map_get(&s->src->vertices, &i);
		const u32 inext = i < n - 1 ? i + 1 : 0;
		const v2 * a = array_get(&s->src->vertices, i);
		const v2 * b = array_get(&s->src->vertices, inext);
		const v2 * c = ss_vertex(s, dst->dst_idx0);
		v2 ab, ab_perp, bc;
		v2_sub(b, a, &ab);
		v2_perp(&ab, false, &ab_perp); // not always true - cc?
		ab_normalize(&ab_perp, &ab_perp);
		v2_sub(c, b, &bc);
		const r32 d = v2_dot(&ab, &bc);
		if (d <= inset)
		{
			v2 ac;
			v2_sub(c, a, &ac);
			v2_normalize(&ac, &ac);
			
		}
		else
		{
			
		}
	}

	const u32 n = s->src->vertices.size;
	array_map_iter it = {0};
	while (array_map_iterate(&s->edges, &it))
	{
		const u32 * src_idx = it.key;
		const ss_edge_pair * dst = it.val;

		const u32 next_idx = i < n - 1 ? i + 1 : 0;
		const v2 * a = array_get(&s->src->vertices, src_idx);
		const v2 * b = array_get(&s->src->vertices, next_idx);
		const v2 * c = ss_vertex(s, dst->dst_idx0);
		v2 ab, ab_perp, bc;
		v2_sub(b, a, &ab);
		v2_perp(&ab, true, &ab_perp); // not always true - cc?
		ab_normalize(&ab_perp, &ab_perp);
		v2_sub(c, b, &bc);
		const r32 d = v2_dot(&ab, &bc);
		if (d <= inset)
		{
			
		}

		if (dst->dst_idx1 != UINT_MAX)
		{
			
		}
	}
}*/

