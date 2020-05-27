#ifndef VIOLET_ALL_H
#define VIOLET_ALL_H

/* Core */
#include "violet/core.h"
/* Data structures */
#include "violet/array.h"
#include "violet/list.h"
/* Math */
#include "violet/dmath.h"
#include "violet/fmath.h"
#include "violet/imath.h"
#include "violet/geom.h"
/* Serialization */
#include "violet/vson.h"
#include "violet/base64.h"
/* String */
#include "violet/string.h"
#include "violet/localize.h"
#include "violet/utf8.h"
/* OS */
#include "violet/os.h"
/* Profiler */
#include "violet/profiler.h"
/* Gui */
#include "violet/color.h"
#ifndef VIOLET_NO_GUI
#include "violet/gui.h"
#include "violet/sdl_gl.h"
#else
#include "violet/stbi.h"
#endif
/* Audio */
#include "violet/audio.h"

/* Additional utilities useful when using the entire library */

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

static inline
v2f v2d_to_v2f(v2d v)
{
	return (v2f){ .x = v.x, .y = v.y };
}

static inline
v2d v2f_to_v2d(v2f v)
{
	return (v2d){ .x = v.x, .y = v.y };
}

#endif // VIOLET_ALL_H

#ifdef VIOLET_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#define AUDIO_IMPLEMENTATION
#define BASE64_IMPLEMENTATION
#define COLOR_IMPLEMENTATION
#define CORE_IMPLEMENTATION
#define DMATH_IMPLEMENTATION
#define FMATH_IMPLEMENTATION
#define GEOM_IMPLEMENTATION
#define GUI_IMPLEMENTATION
#define IMATH_IMPLEMENTATION
#define LIST_IMPLEMENTATION
#define LOCALIZE_IMPLEMENTATION
#define OS_IMPLEMENTATION
#define PROFILER_IMPLEMENTATION
#define SDL_GL_IMPLEMENTATION
#define STRING_IMPLEMENTATION
#define UTF8_IMPLEMENTATION
#define VSON_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#undef VIOLET_IMPLEMENTATION
#endif

/* Core */
#include "violet/core.h"
/* Data structures */
#include "violet/array.h"
#include "violet/list.h"
/* Math */
#include "violet/dmath.h"
#include "violet/fmath.h"
#include "violet/imath.h"
#include "violet/geom.h"
/* Serialization */
#include "violet/vson.h"
#include "violet/base64.h"
/* String */
#include "violet/string.h"
#include "violet/localize.h"
#include "violet/utf8.h"
/* OS */
#include "violet/os.h"
/* Profiler */
#include "violet/profiler.h"
/* Gui */
#include "violet/color.h"
#ifndef VIOLET_NO_GUI
#include "violet/gui.h"
#include "violet/sdl_gl.h"
#else
#include "violet/stbi.h"
#endif
/* Audio */
#include "violet/audio.h"
