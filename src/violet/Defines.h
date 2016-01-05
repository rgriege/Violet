#ifndef VIOLET_Defines_H
#define VIOLET_Defines_H

#include "violet/handle/Handle.h"

typedef unsigned char uint8;
typedef uint8 ubyte;
typedef char int8;
// typedef int8 byte;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned uint32;
typedef int int32;
typedef long long unsigned uint64;
typedef long long int64;

namespace Violet
{
	typedef Handle<65536, 65536> EntityId;
}

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
