#ifndef VIOLET_ALL_H
#define VIOLET_ALL_H

#ifdef VIOLET_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#define CORE_IMPLEMENTATION
#define DMATH_IMPLEMENTATION
#define FMATH_IMPLEMENTATION
#define GUI_IMPLEMENTATION
#define IMATH_IMPLEMENTATION
#define OS_IMPLEMENTATION
#define VSON_IMPLEMENTATION
#undef VIOLET_IMPLEMENTATION
#endif

#include "violet/core.h"
#include "violet/array.h"
#include "violet/dmath.h"
#include "violet/fmath.h"
#include "violet/gui.h"
#include "violet/imath.h"
#include "violet/os.h"
#include "violet/vson.h"


static inline
v2f v2i_to_v2f(v2i v)
{
	return (v2f){ .x = v.x, .y = v.y };
}

static inline
v2i v2f_to_v2i(v2f v)
{
	return (v2i){ .x = v.x, .y = v.y };
}

#endif // VIOLET_ALL_H
