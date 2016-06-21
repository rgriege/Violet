#ifndef _MATH_INTERFACE_END

#define IVAL CONCAT(ival, SUFFIX)
#define IVAL_(name) CONCAT(IVAL, CONCAT(_, name))
#define IVALG_(name) CONCAT(g_, IVAL_(name))

typedef struct IVAL
{
	SCALAR l, r;
} IVAL;

extern const IVAL IVALG_(0_to_1);

void IVAL_(slide)(IVAL *i, SCALAR d);
SCALAR IVAL_(length)(const IVAL *i);

b8 IVAL_(contains_val)(const IVAL *i, SCALAR x);
b8 IVAL_(contains_ival)(const IVAL *lhs, const IVAL *rhs);

b8 IVAL_(overlaps)(const IVAL *lhs, const IVAL *rhs);
SCALAR IVAL_(overlap)(const IVAL *lhs, const IVAL *rhs);

#else // _MATH_INTERFACE_END
#undef IVAL
#undef IVAL_
#undef IVALG_
#endif // _MATH_INTERFACE_END
