#ifndef VIOLET_Defines_H
#define VIOLET_Defines_H

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

#ifdef WIN32
#ifdef VIOLETCORE_EXPORT
#define VIOLET_API __declspec(dllexport)
#else
#define VIOLET_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

#endif
