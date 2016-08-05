#ifndef VIOLET_VSON_H
#define VIOLET_VSON_H

#include "violet/core/types.h"

typedef struct istream istream;
typedef struct ostream ostream;

b8 vson_read_b8(istream *s, const char *label, b8 *val);
b8 vson_read_s32(istream *s, const char *label, s32 *val);
b8 vson_read_u32(istream *s, const char *label, u32 *val);
b8 vson_read_r32(istream *s, const char *label, r32 *val);
b8 vson_read_str(istream *s, const char *label, char *val, u32 sz);

void vson_write_b8(ostream *s, const char *label, b8 val);
void vson_write_s32(ostream *s, const char *label, s32 val);
void vson_write_u32(ostream *s, const char *label, u32 val);
void vson_write_r32(ostream *s, const char *label, r32 val);
void vson_write_str(ostream *s, const char *label, const char *val);

#endif

