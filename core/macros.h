#ifndef VIOLET_MACROS_H
#define VIOLET_MACROS_H

#ifdef DEBUG
#define VLT_ASSERT_DBG(x) assert(x)
#else
#define VLT_ASSERT_DBG(x) ((void)0)
#endif

#define UNUSED(x) ((void)(x))

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)

#define VLT_MACRO_FUNC_DECLARE(x)

#endif
