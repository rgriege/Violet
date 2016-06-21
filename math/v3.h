#ifndef _MATH_INTERFACE_END

#define V3 CONCAT(v3, SUFFIX)
#define V3_(name) CONCAT(V3, CONCAT(_, name))
#define V3G_(name) CONCAT(g_, V3_(name))

typedef struct V3
{
	SCALAR x, y, z;
} V3;

extern const V3 V3G_(x_axis);
extern const V3 V3G_(y_axis);
extern const V3 V3G_(z_axis);
extern const V3 V3G_(zero);

SCALAR V3_(mag)(const V3 *v);
SCALAR V3_(mag_squared)(const V3 *v);
void V3_(normalize)(const V3 *v, V3 *res);
b8 V3_(is_unit)(const V3 *v);
void V3_(scale)(const V3 *v, SCALAR sx, SCALAR sy, SCALAR sz, V3 *res);

void V3_(add)(const V3 *lhs, const V3 *rhs, V3 *res);
void V3_(sub)(const V3 *lhs, const V3 *rhs, V3 *res);

SCALAR V3_(dot)(const V3 *lhs, const V3 *rhs);
void V3_(cross)(const V3 *lhs, const V3 *rhs, V3 *res);
void V3_(proj)(const V3 *v, const V3 *axis, V3 *res);
void V3_(inverse)(const V3 *v, V3 *res);
b8 V3_(is_zero)(const V3 *v);
b8 V3_(equal)(const V3 *lhs, const V3 *rhs);

#else // _MATH_INTERFACE_END
#undef V3
#undef V3_
#undef V3G_
#endif // _MATH_INTERFACE_END

