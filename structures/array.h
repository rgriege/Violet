#ifndef VIOLET_ARRAY_H
#define VIOLET_ARRAY_H

#include "violet/core/types.h"

typedef struct array
{
	void *data;
	u32 elem_size;
	u32 size;
	u32 capacity;
} array;

void array_init(array *a, u32 elem_size);
void _array_copy_prepare(array *dst, const array *src);
void array_copy(array *dst, const array *src);
#define array_copy_deep(dst, src, copy_elem) \
	_array_copy_prepare(dst, src); \
	for (u32 _i = 0; _i < (src)->size; ++_i) \
		copy_elem(array_get(dst, _i), array_get(src, _i));
void array_move(array *dst, array *src);
void array_destroy(array *a);
#define array_destroy_each(a, destroy_elem) \
	for (u32 _i = 0; _i < (a)->size; ++_i) \
		destroy_elem(array_get(a, _i));
#define array_destroy_deep(a, destroy_elem) \
	array_destroy_each(a, destroy_elem); \
	array_destroy(a);

void *array_get(const array *a, u32 idx);
void *array_first(const array *a);
void *array_last(const array *a);

u32 array_size(const array *a);
b8 array_empty(const array *a);
u32 array_capacity(const array *a);

void array_remove(array *a, u32 idx);
void array_clear(array *a);
void *array_append(array *a, const void *elem);
void *array_append_null(array *a);
void *array_append_zero(array *a);
void array_insert(array *a, u32 idx, const void *elem);
void *array_insert_null(array *a, u32 idx);
void array_reserve(array *a, u32 capacity);

void array_reverse(array *a);
void array_qsort(array *a, int(*comp)(const void *, const void *));
void *array_bsearch(array *a, const void *key,
                    int(*comp)(const void *, const void *));
b8 array_contains(const array *a, const void *elem);
void *array_upper(array *a, const void *key,
                  int(*comp)(const void *, const void*));

#endif
