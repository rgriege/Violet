#ifndef _MATH_INTERFACE_END

#define M2 CONCAT(m2, SUFFIX)
#define M2_(name) CONCAT(M2, CONCAT(_, name))
#define M2G_(name) CONCAT(g_, M2_(name))

typedef SCALAR M2[4];

extern const M2 M2G_(identity);
extern const M2 M2G_(zero);

void M2_(init_rot)(M2 m, SCALAR radians);

b8 M2_(inverse)(const M2 m, M2 result);
SCALAR M2_(determinant)(const M2 m);

void M2_(scale)(const M2 m, SCALAR s, M2 result);
void M2_(mul_v2)(const M2 m, const V2 *v, V2 *result);

b8 M2_(equal)(const M2 lhs, const M2 rhs);

#else // _MATH_INTERFACE_END
#undef M2
#undef M2_
#undef M2G_
#endif // _MATH_INTERFACE_END

