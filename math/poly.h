#ifndef _MATH_INTERFACE_END
#include "violet/structures/array.h"

#define POLY CONCAT(poly, SUFFIX)
#define POLY_(name) CONCAT(POLY, CONCAT(_, name))

void POLY_(init)(array *p);
void POLY_(from_box)(array *p, const BOX2 *box);
void POLY_(destroy)(array *p);

b8 POLY_(is_simple)(const array *p);
b8 POLY_(is_convex)(const array *p);

b8 POLY_(contains)(const array *p, const V2 *point);
void POLY_(bounding_box)(const array *p, BOX2 *box);

void POLY_(translate)(array *p, const V2 *delta);

IVAL POLY_(project)(const array *p, const V2 *axis);
V2 POLY_(center)(const array *p);
SCALAR POLY_(area)(const array *p);
V2 POLY_(centroid)(const array *p);
b8 POLY_(is_cc)(const array *p);
b8 POLY_(segment_intersect)(const array *p, const V2 *v0, const V2 *v1);

#else // _MATH_INTERFACE_END
#undef POLY_
#undef POLY
#endif // _MATH_INTERFACE_END
