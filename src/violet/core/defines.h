#ifndef VIOLET_DEFINES_H
#define VIOLET_DEFINES_H

#include "violet/core/types.h"

#ifdef WIN32

#define TEMPLATE_HINT

#ifdef VIOLETCORE_EXPORT
#define VIOLET_API __declspec(dllexport)
#else // VIOLETCORE_EXPORT
#define VIOLET_API __declspec(dllimport)
#endif // VIOLETCORE_EXPORT

#else // Win32

#define TEMPLATE_HINT template

#define VIOLET_API

#endif // Win32

#define thread_const const
#define thread_mutable mutable

#endif
