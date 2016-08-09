#ifdef _MATH_INTERFACE

void MATH_(line_extrapolate)(const V2 *a, const V2 *b, SCALAR t, V2 *p);
b8 MATH_(ray_intersect_coords)(const V2 *a, const V2 *adir,
                               const V2 *b, const V2 *bdir,
                               SCALAR *t, SCALAR *u);
b8 MATH_(line_intersect_coords)(const V2 *a0, const V2 *a1,
                                const V2 *b0, const V2 *b1,
                                SCALAR *t, SCALAR *u);
b8 MATH_(line_intersect)(const V2 *a0, const V2 *a1,
                         const V2 *b0, const V2 *b1, V2 *isec);
b8 MATH_(segment_intersect)(const V2 *a0, const V2 *a1,
                            const V2 *b0, const V2 *b1, V2 *isec);
void MATH_(nearest_point_on_segment)(const V2 *a, const V2 *b, const V2 *p,
                                     V2 *result);
void MATH_(nearest_point_on_line)(const V2 *a, const V2 *b, const V2 *p,
                                  V2 *result);
SCALAR MATH_(point_to_segment_dist)(const V2 *a, const V2 *b, const V2 *p);
SCALAR MATH_(point_to_line_dist)(const V2 *a, const V2 *b, const V2 *p);

#endif // _MATH_INTERFACE

