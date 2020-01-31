#ifndef VIOLET_GEOM_H
#define VIOLET_GEOM_H

/* always counter-clockwise; angles must be between 0 and 2*pi radians */
u32  arc_poly_sz(r32 r, r32 angle_start, r32 angle_end);
void arc_to_poly(r32 x, r32 y, r32 r, r32 angle_start, r32 angle_end,
                 v2f *v, u32 segments, b32 closed);

u32 triangulate_out_sz(u32 n);
u32 triangulate_reserve_sz(u32 n);
/* triangles buffer must be at least triangulate_reserve_sz() long */
b32 triangulate(const v2f *v, u32 n, v2f *triangles);
b32 triangulatea(const v2f *v, u32 n, array(v2f) *triangles);


#endif // VIOLET_GEOM_H

/* Implementation */

#ifdef GEOM_IMPLEMENTATION

u32 arc_poly_sz(r32 r, r32 angle_start, r32 angle_end)
{
	const r32 angle_delta = angle_end > angle_start
	                      ? angle_end - angle_start
	                      : angle_end - angle_start + 2.f*fPI;
	return (u32)((2.f + r) * (angle_delta / fPI));
}

void arc_to_poly(r32 x, r32 y, r32 r, r32 angle_start, r32 angle_end,
                 v2f *v, u32 segments, b32 closed)
{
	const r32 angle_delta = angle_end > angle_start
	                      ? angle_end - angle_start
	                      : angle_end - angle_start + 2.f*fPI;
	const r32 radians_slice = angle_delta / (segments - !closed);
	for (u32 i = 0; i < segments; ++i) {
		const r32 radians = angle_start + radians_slice * i;
		v[i] = (v2f){ .x=x+r*cosf(radians), .y=y+r*sinf(radians) };
	}
}


/* Polygon triangulation algorithm
 * Modified from John W. Ratcliff
 * www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml */
static
b32 triangle__contains(v2f a, v2f b, v2f c, v2f p)
{
	const r32 ab_cross_ap = v2f_cross(v2f_sub(b, a), v2f_sub(p, a));
	const r32 bc_cross_bp = v2f_cross(v2f_sub(c, b), v2f_sub(p, b));
	const r32 ca_cross_cp = v2f_cross(v2f_sub(a, c), v2f_sub(p, c));
	return ab_cross_ap >= 0.f && bc_cross_bp >= 0.f && ca_cross_cp >= 0.f;
}

static
b32 triangulate__snip(const v2f *poly, u32 u, u32 v, u32 w, u32 n)
{
	const v2f a = poly[u];
	const v2f b = poly[v];
	const v2f c = poly[w];
	static const float EPSILON = 0.0000000001f;

	/* cannot snip if b is a concave vertex */
	if ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) < EPSILON)
		return false;

	/* cannot snip if triangle abc contains another (concave) vtx in poly */
	for (u32 i = 0; i < n; ++i)
		if (   i != u && i != v && i != w
		    && !v2f_equal(poly[i], a)
		    && !v2f_equal(poly[i], b)
		    && !v2f_equal(poly[i], c)
		    && triangle__contains(a, b, c, poly[i]))
			return false;
	return true;
}

u32 triangulate_out_sz(u32 n)
{
	return 3 * (n - 2);
}

u32 triangulate_reserve_sz(u32 n)
{
	return triangulate_out_sz(n) + n;
}

b32 triangulate(const v2f *v, u32 n, v2f *triangles)
{
	v2f *v_mut;
	u32 n_cur = n;
	u32 out = 0;

	/* Copy the original poly into the last verts of triangle buffer
	 * so we can remove the vertices later. */
	v_mut = &(triangles)[triangulate_out_sz(n)];
	memcpy(v_mut, v, n * sizeof(*v));

	if (polyf_is_cw(v, n))
		reverse(v_mut, sizeof(*v), n);

	while (n_cur > 2) {
		const u32 n_prev = n_cur;
		for (u32 i = 0; i < n_cur; ++i) {
			/* iterate in reverse order to likely reduce cost of vtx removal */
			const u32 a = (2 * n_cur - i - 2) % n_cur;
			const u32 b = n_cur - i - 1;
			const u32 c = (n_cur - i) % n_cur;
			if (triangulate__snip(v_mut, a, b, c, n_cur)) {
				triangles[out+0] = v_mut[a];
				triangles[out+1] = v_mut[b];
				triangles[out+2] = v_mut[c];
				out += 3;

				buf_remove(v_mut, b, n_cur);
				--n_cur;
				break;
			}
		}
		if (n_prev == n_cur)
			return false;
	}

	return true;
}

b32 triangulatea(const v2f *v, u32 n, array(v2f) *triangles)
{
	const u32 prev_triangle_cnt = array_sz(*triangles);
	array_reserve(*triangles, prev_triangle_cnt + triangulate_reserve_sz(n));
	if (triangulate(v, n, &(*triangles)[prev_triangle_cnt])) {
		array_set_sz(*triangles, prev_triangle_cnt + triangulate_out_sz(n));
		return true;
	}
	return false;
}

#undef GEOM_IMPLEMENTATION
#endif // GEOM_IMPLEMENTATION
