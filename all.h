#ifndef VIOLET_ALL_H
#define VIOLET_ALL_H

#ifdef VIOLET_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#define CORE_IMPLEMENTATION
#define FMATH_IMPLEMENTATION
#define GUI_IMPLEMENTATION
#define IMATH_IMPLEMENTATION
#define OS_IMPLEMENTATION
#define VSON_IMPLEMENTATION
#undef VIOLET_IMPLEMENTATION
#endif

#include "violet/core.h"
#define array_size_t u32
#include "violet/array.h"
#include "violet/fmath.h"
#include "violet/gui.h"
#include "violet/imath.h"
#include "violet/os.h"
#include "violet/vson.h"

#endif // VIOLET_ALL_H
