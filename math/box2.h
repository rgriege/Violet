#ifdef _MATH_DEFINES

#define BOX2 CONCAT(box2, SUFFIX)
#define BOX2_(name) CONCAT(BOX2, CONCAT(_, name))

#endif // _MATH_DEFINES

#ifdef _MATH_INTERFACE

typedef struct BOX2
{
	V2 min;
	V2 max;
} BOX2;

void BOX2_(init_point)(BOX2 *b, const V2 *p);
void BOX2_(from_line)(BOX2 *b, const V2 *start, const V2 *end);
void BOX2_(from_center)(BOX2 *b, const V2 *center, const V2 *half_dim);
void BOX2_(from_dims)(BOX2 *b, SCALAR left, SCALAR top,
                      SCALAR right, SCALAR bottom);

b8 BOX2_(empty)(const BOX2 *b);

b8 BOX2_(contains_point)(const BOX2 *b, const V2 *p);
b8 BOX2_(contains_box)(const BOX2 *lhs, const BOX2 *rhs);

void BOX2_(clamp_point)(const BOX2 *b, V2 *p);

b8 BOX2_(overlaps)(const BOX2 *lhs, const BOX2 *rhs);
b8 BOX2_(eq)(const BOX2 *lhs, const BOX2 *rhs);

void BOX2_(extend_point)(BOX2 *b, const V2 *p);
void BOX2_(extend_box)(BOX2 *b, const BOX2 *other);

void BOX2_(translate)(BOX2 *b, const V2 *v);
void BOX2_(transform)(BOX2 *b, const M3 mat);

void BOX2_(get_center)(const BOX2 *b, V2 *center);
void BOX2_(get_half_dim)(const BOX2 *b, V2 *half_dim);

#endif // _MATH_INTERFACE

#ifdef _MATH_UNDEFINES
#undef BOX2_
#undef BOX2
#endif // _MATH_UNDEFINES

