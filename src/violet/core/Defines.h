#ifndef DEFINES_H
#define DEFINES_H

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned uint32;
typedef int int32;

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