#ifndef VIOLET_ARRAY_H
#define VIOLET_ARRAY_H

#include "violet/core/types.h"

typedef struct array
{
	void * data;
	u32 elem_size;
	u32 size;
	u32 capacity;
} array;

void array_init(array * a, u32 elem_size);
void array_copy(array * dest, const array * src);
void array_destroy(array * a);
void array_destroy_each(array * a, void (*destroy_elem)(void*));

void * array_get(const array * a, u32 idx);
void * array_first(array * a);
void * array_last(array * a);

u32 array_size(const array * a);
b8 array_empty(const array * a);
u32 array_capacity(const array * a);

void array_remove(array * a, u32 idx);
void array_clear(array * a);
void * array_append(array * a, const void * elem);
void * array_append_null(array * a);
void * array_append_zero(array * a);
void * array_insert_null(array * a, u32 idx);
void array_reserve(array * a, u32 capacity);

#endif
