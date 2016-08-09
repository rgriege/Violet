#ifdef _MATH_DEFINES

#define M3 CONCAT(m3, SUFFIX)
#define M3_(name) CONCAT(M3, CONCAT(_, name))
#define M3G_(name) CONCAT(g_, M3_(name))

#endif // _MATH_DEFINES

#ifdef _MATH_INTERFACE

typedef SCALAR M3[9];

extern const M3 M3G_(identity);
extern const M3 M3G_(zero);

void M3_(init_rot)(M3 m, r32 radians);

void M3_(mul_m3)(const M3 lhs, const M3 rhs, M3 res);
void M3_(mul_v2)(const M3 m, const V2 *v, V2 *res);
void M3_(mul_v3)(const M3 m, const V3 *v, V3 *res);

b8 M3_(equal)(const M3 lhs, const M3 rhs);

#endif // _MATH_INTERFACE

#ifdef _MATH_UNDEFINES
#undef M3G_
#undef M3_
#undef M3
#endif // _MATH_UNDEFINES

