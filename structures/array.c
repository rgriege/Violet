#include <stdlib.h>
#include <string.h>

#include "violet/math/math.h"
#include "violet/structures/array.h"

void array_init(array * a, u32 elem_size)
{
	a->data = NULL;
	a->elem_size = elem_size;
	a->size = 0;
	a->capacity = 0;
}

void array_destroy(array * a)
{
	free(a->data);
	memset(a, 0, sizeof(array));
}

void * array_get(array * a, u32 idx)
{
	return a->data + idx * a->elem_size;
}

void * array_first(array * a)
{
	return a->data;
}

void * array_last(array * a)
{
	return a->data + a->size * a->elem_size;
}

b8 array_empty(const array * a)
{
	return a->size == 0;
}

void array_clear(array * a)
{
	a->size = 0;
}

void * array_append(array * a, void * elem)
{
	if (a->size >= a->capacity)
		array_reserve(a, max(a->capacity * 3 / 2, a->capacity + 1));
	void * p = a->data + a->size++ * a->elem_size;
	memcpy(p, elem, a->elem_size);
	return p;
}

void array_reserve(array * a, u32 capacity)
{
	if (capacity > a->capacity)
	{
		void * data = malloc(capacity * a->elem_size);
		memcpy(data, a->data, a->size * a->elem_size);
		free(a->data);
		a->data = data;
		a->capacity = capacity;
	}
}
