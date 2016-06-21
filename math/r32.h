#ifndef VIOLET_R32MATH_H
#define VIOLET_R32MATH_H

#include "violet/core/macros.h"
#include "violet/core/types.h"
#include "violet/math/common.h"

#define SCALAR r32
#define SUFFIX f

#include "violet/math/math.h"
#include "violet/math/v2.h"
#include "violet/math/v3.h"
#include "violet/math/m2.h"
#include "violet/math/m3.h"
#include "violet/math/ival.h"
#include "violet/math/line.h"
#include "violet/math/box2.h"
#include "violet/math/poly.h"

#ifndef _MATH_IMPLEMENTATION
#define _MATH_INTERFACE_END

#include "violet/math/math.h"
#include "violet/math/v2.h"
#include "violet/math/v3.h"
#include "violet/math/m2.h"
#include "violet/math/m3.h"
#include "violet/math/ival.h"
#include "violet/math/line.h"
#include "violet/math/box2.h"
#include "violet/math/poly.h"

#undef _MATH_INTERFACE_END

#undef SUFFIX
#undef SCALAR

#endif // _MATH_IMPLEMENTATION

#endif // VIOLET_R32MATH_H
