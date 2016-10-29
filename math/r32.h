#ifndef VIOLET_R32MATH_H
#define VIOLET_R32MATH_H
#define _MATH_DEFINES
#define _MATH_INTERFACE
#elif defined(_MATH_EXT)
#define _MATH_DEFINES
#endif // VIOLET_R32MATH_H

#ifdef _MATH_DEFINES

#include <float.h>

#include "violet/core/macros.h"
#include "violet/core/types.h"
#include "violet/math/common.h"

#define SCALAR r32
#define SUFFIX f
#define ABS fabsf
#define SCALAR_MIN FLT_MIN
#define SCALAR_MAX FLT_MAX
#define HAS_DECIMAL

#include "violet/math/templates.h"

#undef _MATH_DEFINES
#endif // _MATH_DEFINES

#ifdef _MATH_INTERFACE
#undef _MATH_INTERFACE
#endif // _MATH_INTERFACE

#if !defined(_MATH_IMPLEMENTATION) && !defined(_MATH_EXT)
#define _MATH_UNDEFINES

#include "violet/math/templates.h"

#undef _MATH_UNDEFINES

#undef SUFFIX
#undef SCALAR
#undef ABS
#undef SCALAR_MIN
#undef SCALAR_MAX
#undef HAS_DECIMAL

#endif // _MATH_IMPLEMENTATION

