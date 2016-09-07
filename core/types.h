#ifndef VIOLET_TYPES_H
#define VIOLET_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef bool b8;
typedef unsigned char u8;
typedef u8 ubyte;
typedef char s8;
typedef s8 byte;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float r32;
typedef double r64;

typedef union uintptr
{
	uintptr_t i;
	void *p;
} uintptr;

#endif
