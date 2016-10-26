#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "violet/core/macros.h"
#include "violet/math/common.h"
#include "violet/structures/array.h"

#ifdef DEBUG
#define ARRAY_MALLOC(num, sz) calloc((num),(sz))
#else
#define ARRAY_MALLOC(num, sz) malloc((num)*(sz))
#endif

void array_init(array *a, u32 elem_size)
{
	a->data = NULL;
	a->elem_size = elem_size;
	a->size = 0;
	a->capacity = 0;
}

void _array_copy_prepare(array *dst, const array *src)
{
	VLT_ASSERT_DBG(dst->data == NULL);

	dst->elem_size = src->elem_size;
	dst->size = src->size;
	dst->capacity = src->capacity;
	dst->data = ARRAY_MALLOC(dst->capacity, dst->elem_size);
}

void array_copy(array *dst, const array *src)
{
	_array_copy_prepare(dst, src);
	memcpy(dst->data, src->data, dst->size * dst->elem_size);
}

void array_move(array *dst, array *src)
{
	VLT_ASSERT_DBG(dst->data == NULL);

	memcpy(dst, src, sizeof(array));
	src->data = NULL;
	src->size = 0;
	src->capacity = 0;
}

void array_destroy(array *a)
{
	if (a->capacity > 0)
	{
		free(a->data);
		memset(a, 0, sizeof(array));
	}
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
		void *data = ARRAY_MALLOC(capacity, a->elem_size);
		memcpy(data, a->data, a->size * a->elem_size);
		free(a->data);
		a->data = data;
		a->capacity = capacity;
	}
}

void array_reverse(array *a)
{
	ubyte *scratch = malloc(a->elem_size);
	for (u32 i = 0, n = a->size / 2; i < n; ++i)
	{
		void *l = array_get(a, i);
		void *r = array_get(a, a->size - 1 - i);
		memcpy(scratch, r, a->elem_size);
		memcpy(r, l, a->elem_size);
		memcpy(l, scratch, a->elem_size);
	}
	free(scratch);
}

void array_qsort(array *a, int(*comp)(const void *, const void *))
{
	qsort(a->data, a->size, a->elem_size, comp);
}

void *array_bsearch(array *a, const void *key,
                    int(*comp)(const void *, const void *))
{
	return bsearch(key, a->data, a->size, a->elem_size, comp);
}

b8 array_contains(const array *a, const void *elem)
{
	for (u32 i = 0; i < a->size; ++i)
		if (memcmp(elem, array_get(a, i), a->elem_size) == 0)
			return true;
	return false;
}

void *array_upper(array *a, const void *key,
                  int(*comp)(const void *, const void*))
{
	u32 left = 0, right = a->size, mid = a->size/2;
	void *res = NULL;
	while (left != right)
	{
		void *p = array_get(a, mid);
		if (comp(key, p) < 0)
		{
			res = p;
			right = mid;
		}
		else
			left = mid+1;
		mid = (right+left)/2;
	}
	return res;
}

