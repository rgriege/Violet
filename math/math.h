#ifndef _MATH_INTERFACE_END

#define MATH CONCAT(math, SUFFIX)
#define MATH_(name) CONCAT(MATH, CONCAT(_, name))

SCALAR MATH_(clamp)(SCALAR lo, SCALAR val, SCALAR hi);

SCALAR MATH_(deg2rad)(SCALAR deg);
SCALAR MATH_(rad2deg)(SCALAR rad);

#else // _MATH_INTERFACE_END
#undef MATH
#undef MATH_
#endif // _MATH_INTERFACE_END
