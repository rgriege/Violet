#ifndef VIOLET_DECOMPOSE_H
#define VIOLET_DECOMPOSE_H

typedef struct array array;
typedef struct v2f v2f;

void polyf_decompose(const v2f *v, u32 n, array *polys);

#endif

