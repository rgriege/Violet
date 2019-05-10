#ifndef VIOLET_GEOM_H
#define VIOLET_GEOM_H

/* always counter-clockwise; angles must be between 0 and 2*pi radians */
u32  arc_poly_sz(r32 r, r32 angle_start, r32 angle_end);
void arc_to_poly(r32 x, r32 y, r32 r, r32 angle_start, r32 angle_end,
                 v2f *v, u32 segments, b32 closed);

u32 triangulate_out_sz(u32 n);
b32 triangulate(const v2f *v, u32 n, array(v2f) *triangles);


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

b32 triangulate(const v2f *v_, u32 n_, v2f **triangles)
{
	const u32 prev_triangle_cnt = array_sz(*triangles);
	const u32 out_vtx_cnt = triangulate_out_sz(n_);
	v2f *v;
	u32 n;

	array_reserve(*triangles, prev_triangle_cnt + out_vtx_cnt + n_);

	/* Copy the original poly into the last verts of triangle buffer
	 * so we can remove the vertices later. */
	v = &(*triangles)[prev_triangle_cnt + out_vtx_cnt];
	memcpy(v, v_, n_ * sizeof(*v_));
	n = n_;

	if (!polyf_is_cc(v_, n_))
		reverse(v, sizeof(*v_), n);

	while (n > 2) {
		const u32 old_n = n;
		for (u32 i = 0; i < n; ++i) {
			/* iterate in reverse order to likely reduce cost of vtx removal */
			const u32 a = (2 * n - i - 2) % n, b = n - i - 1, c = (n - i) % n;
			if (triangulate__snip(v, a, b, c, n)) {
				array_append(*triangles, v[a]);
				array_append(*triangles, v[b]);
				array_append(*triangles, v[c]);
				buf_remove(v, b, n);
				--n;
				break;
			}
		}
		if (old_n == n) {
			array_set_sz(*triangles, prev_triangle_cnt);
			return false;
		}
	}

	return true;
}

#undef GEOM_IMPLEMENTATION
#endif // GEOM_IMPLEMENTATION
