#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "violet/structures/array_map.h"

void array_map_init(array_map *m, u32 key_size, u32 val_size)
{
	array_init(&m->pairs, key_size + val_size);
	m->key_size = key_size;
}

void array_map_copy(array_map *dst, const array_map *src)
{
	array_copy(&dst->pairs, &src->pairs);
	dst->key_size = src->key_size;
}

void array_map_destroy(array_map *m)
{
	array_destroy(&m->pairs);
}

void array_map_insert(array_map *m, const void *key, const void *val)
{
	void *p = array_map_insert_null(m, key);
	memcpy(p, val, m->pairs.elem_size - m->key_size);
}

void *array_map_insert_null(array_map *m, const void *key)
{
	assert(!array_map_has(m, key));
	void *p = array_append_null(&m->pairs);
	memcpy(p, key, m->key_size);
	return (byte*)p + m->key_size;
}

u32 array_map_size(const array_map *m)
{
	return array_size(&m->pairs);
}

u32 array_map_val_size(const array_map *m)
{
	return m->pairs.elem_size - m->key_size;
}

void *array_map_get(const array_map *m, const void *key)
{
	for (u32 i = 0, n = array_map_size(m); i < n; ++i)
	{
		void *elem = array_get(&m->pairs, i);
		if (memcmp(key, elem, m->key_size) == 0)
			return (byte*)elem + m->key_size;
	}

	return NULL;
}

b8 array_map_has(const array_map *m, const void *key)
{
	return array_map_get(m, key) != NULL;
}

void array_map_remove(array_map *m, const void *key)
{
	assert(array_map_has(m, key));
	for (u32 i = 0, n = array_map_size(m); i < n; ++i)
	{
		void *elem = array_get(&m->pairs, i);
		if (memcmp(key, elem, m->key_size) == 0)
		{
			array_remove(&m->pairs, i);
			return;
		}
	}
}

b8 array_map_iterate(const array_map *m, array_map_iter *iter)
{
	if (iter->next_idx >= array_map_size(m))
		return false;

	iter->key = array_get(&m->pairs, iter->next_idx);
	iter->val = (byte*)iter->key + m->key_size;
	++iter->next_idx;
	return true;
}

void array_map_remove_iter(array_map *m, array_map_iter *iter)
{
	--iter->next_idx;
	assert(iter->next_idx < array_map_size(m));
	array_remove(&m->pairs, iter->next_idx);
}

void array_map_iter_reset(array_map_iter *iter)
{
	memset(iter, 0, sizeof(array_map_iter));
}

