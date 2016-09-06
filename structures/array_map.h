#ifndef VIOLET_ARRAY_MAP_H
#define VIOLET_ARRAY_MAP_H

#include "violet/structures/array.h"
#include "violet/core/macros.h"

typedef struct array_map
{
	array pairs;
	u32 key_size;
} array_map;

void array_map_init(array_map *m, u32 key_size, u32 val_size);
void array_map_copy(array_map *dst, const array_map *src);
VLT_MACRO_FUNC_DECLARE(
void array_map_copy_deep(array_map *dst, const array_map *src,
                         void(copy_val*)(TYPE*, const TYPE*));)
void array_map_destroy(array_map *m);
#define array_map_destroy_each(m, destroy_val) \
	{ \
		array_map_iter _it = {0}; \
		while(array_map_iterate(m, &_it)) \
			destroy_val(_it.val); \
	}
#define array_map_destroy_deep(m, destroy_val) \
	array_map_destroy_each(m, destroy_val); \
	array_map_destroy(m);

void array_map_insert(array_map *m, const void *key, const void *val);
void *array_map_insert_null(array_map *m, const void *key);
u32 array_map_size(const array_map *m);
u32 array_map_val_size(const array_map *m);
void *array_map_get(const array_map *m, const void *key);
b8 array_map_has(const array_map *m, const void *key);
void array_map_remove(array_map *m, const void *key);

typedef struct array_map_iter
{
	void *key;
	void *val;
	u32 next_idx;
} array_map_iter;

b8 array_map_iterate(const array_map *m, array_map_iter *iter);
void array_map_remove_iter(array_map *m, array_map_iter *iter);
void array_map_iter_reset(array_map_iter *iter);


#define array_map_copy_deep(dst, src, copy_val) \
	{ \
		_array_copy_prepare(&(dst)->pairs, &(src)->pairs); \
		(dst)->key_size = (src)->key_size; \
		array_map_iter _src_it = {0}; \
		array_map_iter _dst_it = {0}; \
		while (array_map_iterate(src, &_src_it)) \
		{ \
			assert(array_map_iterate(dst, &_dst_it)); \
			memcpy(_dst_it.key, _src_it.key, (dst)->key_size); \
			copy_val(_dst_it.val, _src_it.val); \
		} \
	}

#endif

