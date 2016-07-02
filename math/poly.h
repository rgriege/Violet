#ifndef _MATH_INTERFACE_END

#define POLY CONCAT(poly, SUFFIX)
#define POLY_(name) CONCAT(POLY, CONCAT(_, name))

void POLY_(from_box)(V2 *v, const BOX2 *box);

b8 POLY_(is_simple)(const V2 *v, u32 n);
b8 POLY_(is_convex)(const V2 *v, u32 n);

b8 POLY_(contains)(const V2 *v, u32 n, const V2 *point);
void POLY_(bounding_box)(const V2 *v, u32 n, BOX2 *box);

void POLY_(translate)(V2 *v, u32 n, const V2 *delta);

IVAL POLY_(project)(const V2 *v, u32 n, const V2 *axis);
V2 POLY_(center)(const V2 *v, u32 n);
SCALAR POLY_(area)(const V2 *v, u32 n);
V2 POLY_(centroid)(const V2 *v, u32 n);
b8 POLY_(is_cc)(const V2 *v, u32 n);
b8 POLY_(segment_intersect)(const V2 *v, u32 n,
                            const V2 *v0, const V2 *v1);
b8 POLY_(intersect)(const V2 *p1, u32 n1, const V2 *p2, u32 n2);

#else // _MATH_INTERFACE_END
#undef POLY_
#undef POLY
#endif // _MATH_INTERFACE_END
