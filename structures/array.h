#ifndef VIOLET_ARRAY_H
#define VIOLET_ARRAY_H

#include "violet/core/types.h"

typedef struct {
	u8 * data;
	u32 elem_size;
	u32 size;
	u32 capacity;
} array;

void array_init(array * a, u32 elem_size);
void array_destroy(array * a);

void * array_get(array * a, u32 index);
void * array_first(array * a);
void * array_last(array * a);

b8 array_empty(const array * a);

void array_clear(array * a);
void * array_append(array * a, void * elem);
void array_reserve(array * a, u32 capacity);

#endif
