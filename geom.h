#ifndef VIOLET_GEOM_H
#define VIOLET_GEOM_H

/* always counter-clockwise; angles must be between 0 and 2*pi radians */
void arc_to_poly(r32 x, r32 y, r32 r, r32 angle_start, r32 angle_end,
                 v2f *v, u32 segments, b32 closed);

/* maximum number of vertices for triangle output */
u32 triangulate_out_sz(u32 n);
u32 triangulate_reserve_sz(u32 n);
/* triangles buffer must be at least triangulate_reserve_sz() long */
b32 triangulate(const v2f *v, u32 n, v2f *triangles, u32 *n_verts);
b32 triangulatea(const v2f *v, u32 n, array(v2f) *triangles);


#endif // VIOLET_GEOM_H

/* Implementation */

#ifdef GEOM_IMPLEMENTATION

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

/* Check if the point is on the correct side (within error) of each edge of
 * the triangle. This is checked using the point-normal form of the plane
 * pointing inwards from each edge. Points are distances since this is 2D.
 *
 * This function would be much easier to call if it took in the
 * triangle's vertices, but doing it this way is faster since it
 * allows us to hoist most of the computation out of the inner loop. */
static inline
b32 triangle__contains(v2f normal0, r32 point0,
                       v2f normal1, r32 point1,
                       v2f normal2, r32 point2,
                       v2f p, r32 epsilon)
{
	return v2f_dot(normal0, p) >= point0 - epsilon
	    && v2f_dot(normal1, p) >= point1 - epsilon
	    && v2f_dot(normal2, p) >= point2 - epsilon;
}

static
b32 triangulate__snip(const v2f *poly, u32 u, u32 v, u32 w, u32 n)
{
	const v2f a = poly[u];
	const v2f b = poly[v];
	const v2f c = poly[w];
	const r32 epsilon = 0.0000001f;

	/* cannot snip if b is a concave vertex */
	if (v2f_cross(v2f_sub(b, a), v2f_sub(c, b)) < epsilon)
		return false;

	/* cannot snip if triangle abc contains another (concave) vtx in poly */
	const v2f perp0 = v2f_lperp(v2f_dir(a, b));
	const r32 dp0   = v2f_dot(perp0, a);
	const v2f perp1 = v2f_lperp(v2f_dir(b, c));
	const r32 dp1   = v2f_dot(perp1, b);
	const v2f perp2 = v2f_lperp(v2f_dir(c, a));
	const r32 dp2   = v2f_dot(perp2, c);
	for (u32 i = 0; i < n; ++i)
		if (   i != u && i != v && i != w
#if 1
		/** Supports some degenerate polygons with overlapping edges, like ][.
		 * NOTE(ben) this was deemend unnecessary and disabled for a while, but
		 * the increased support for holes in TestFit solvers has made this
		 * necessary again. Refer to eb561726 for the disabling commit.
		 */
		    && !v2f_equal(poly[i], a) && !v2f_equal(poly[i], b) && !v2f_equal(poly[i], c)
#endif
		    && triangle__contains(perp0, dp0, perp1, dp1, perp2, dp2, poly[i], epsilon))
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

b32 triangulate(const v2f *v, u32 n, v2f *triangles, u32 *n_verts)
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
		/* This doesn't necessarily mean failure - it could have triangulated in such
		 * a way that fewer vertices were needed than expected. I have seen a case
		 * where this condition was hit due to all remaining vertices being collinear. */
		if (n_prev == n_cur)
			break;
	}

	*n_verts = out;
	return out >= 3;
}

b32 triangulatea(const v2f *v, u32 n, array(v2f) *triangles)
{
	const u32 prev_vert_cnt = array_sz(*triangles);
	u32 new_vert_cnt = 0;

	array_reserve(*triangles, prev_vert_cnt + triangulate_reserve_sz(n));
	if (triangulate(v, n, &(*triangles)[prev_vert_cnt], &new_vert_cnt)) {
		array_set_sz(*triangles, prev_vert_cnt + new_vert_cnt);
		return true;
	}
	return false;
}

#undef GEOM_IMPLEMENTATION
#endif // GEOM_IMPLEMENTATION
