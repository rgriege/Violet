#ifndef VIOLET_CSON_H
#define VIOLET_CSON_H

#include "violet/core/types.h"

typedef struct istream istream;

b8 vson_read_b8(istream * s, const char * label, b8 * val);
b8 vson_read_s32(istream * s, const char * label, s32 * val);
b8 vson_read_u32(istream * s, const char * label, u32 * val);
b8 vson_read_r32(istream * s, const char * label, r32 * val);
b8 vson_read_str(istream * s, const char * label, char * val, u32 sz);

#endif

