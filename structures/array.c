#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "violet/math/common.h"
#include "violet/structures/array.h"

void array_init(array *a, u32 elem_size)
{
	a->data = NULL;
	a->elem_size = elem_size;
	a->size = 0;
	a->capacity = 0;
}

void array_copy(array *dest, const array *src)
{
	dest->elem_size = src->elem_size;
	dest->size = src->size;
	dest->capacity = src->capacity;
	dest->data = malloc(dest->capacity * dest->elem_size);
	memcpy(dest->data, src->data, dest->size * dest->elem_size);
}

void array_destroy(array *a)
{
	if (a->capacity > 0)
	{
		free(a->data);
		memset(a, 0, sizeof(array));
	}
}

void array_destroy_each_ex(array *a, void (*destroy_elem)(void*))
{
	for (u32 i = 0; i < a->size; ++i)
		destroy_elem(array_get(a, i));
	array_destroy(a);
}

void *array_get(const array *a, u32 idx)
{
	assert(idx < a->size);
	return (byte*)a->data + idx * a->elem_size;
}

void *array_first(const array *a)
{
	assert(a->size > 0);
	return a->data;
}

void *array_last(const array *a)
{
	assert(a->size > 0);
	return (byte*)a->data + (a->size - 1) * a->elem_size;
}

u32 array_size(const array *a)
{
	return a->size;
}

b8 array_empty(const array *a)
{
	return a->size == 0;
}

u32 array_capacity(const array *a)
{
	return a->capacity;
}

void array_remove(array *a, u32 idx)
{
	for (u32 i = idx + 1, n = a->size; i < n; ++i)
		memcpy(array_get(a, i - 1), array_get(a, i), a->elem_size);
	--a->size;
}

void array_clear(array *a)
{
	a->size = 0;
}

void *array_append(array *a, const void *elem)
{
	void *p = array_append_null(a);
	memcpy(p, elem, a->elem_size);
	return p;
}

void *array_append_null(array *a)
{
	if (a->size >= a->capacity)
		array_reserve(a, max(a->capacity * 3 / 2, a->capacity + 1));
	return (byte*)a->data + a->size++ * a->elem_size;
}

void *array_append_zero(array *a)
{
	void *p = array_append_null(a);
	memset(p, 0, a->elem_size);
	return p;
}

void array_insert(array *a, u32 idx, const void *elem)
{
	memcpy(array_insert_null(a, idx), elem, a->elem_size);
}

void *array_insert_null(array *a, u32 idx)
{
	assert(idx <= a->size);
	array_append_null(a);
	for (u32 i = a->size - 1; i > idx; --i)
		memcpy(array_get(a, i), array_get(a, i - 1), a->elem_size);
	return array_get(a, idx);
}

void array_reserve(array *a, u32 capacity)
{
	if (capacity > a->capacity)
	{
		void *data = malloc(capacity * a->elem_size);
		memcpy(data, a->data, a->size * a->elem_size);
		free(a->data);
		a->data = data;
		a->capacity = capacity;
	}
}

void array_reverse(array *a)
{
	ubyte scratch[a->elem_size];
	for (u32 i = 0, n = a->size / 2; i < n; ++i)
	{
		void *l = array_get(a, i);
		void *r = array_get(a, a->size - 1 - i);
		memcpy(scratch, r, a->elem_size);
		memcpy(r, l, a->elem_size);
		memcpy(l, scratch, a->elem_size);
	}
}

