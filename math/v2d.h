#ifndef VIOLET_V2D_H
#define VIOLET_V2D_H

#include "violet/core/types.h"

// TODO(rgriege): use custom impl w/ SIMD

#define V2_SUFFIX d
#define V2_TYPE r64
#include "violet/math/v2def.h"
#undef V2_SUFFIX
#undef V2_TYPE

#endif
