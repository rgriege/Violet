#ifdef _MATH_DEFINES

#define MATH CONCAT(math, SUFFIX)
#define MATH_(name) CONCAT(MATH, CONCAT(_, name))

#endif // _MATH_DEFINES

#ifdef _MATH_INTERFACE

SCALAR MATH_(clamp)(SCALAR lo, SCALAR val, SCALAR hi);

SCALAR MATH_(deg2rad)(SCALAR deg);
SCALAR MATH_(rad2deg)(SCALAR rad);

b8 MATH_(eq)(SCALAR a, SCALAR b, SCALAR err);

#endif // _MATH_INTERFACE

#ifdef _MATH_UNDEFINES
#undef MATH
#undef MATH_
#endif // _MATH_UNDEFINES

