#ifndef VIOLET_ARR_H
#define VIOLET_ARR_H

#include <stdlib.h>

#include "violet/core/macros.h"
#include "violet/core/types.h"
#include "violet/math/common.h"
#include "violet/utility/mem.h"

// TODO(rgriege): replace array

#define arr_raw(a)    (((u32*)(a))-2)
#define arr_create()  _arr_create()
#define arr_free(a)   free(arr_raw(a))

#define arr_cap(a)     (arr_raw(a)[0])
#define arr_sz(a)      (arr_raw(a)[1])
#define arr_raw_sz(a)  (2*sizeof(u32)+arr_sz(a)*sizeof(*a))

#define arr_cp(dst, src) \
	arr_reserve(dst, arr_sz(src)); \
	memcpy(arr_raw(dst)+1, arr_raw(src)+1, arr_sz(a)*sizeof(*a)+sizeof(u32))

#define arr_last(a)               ((a)+arr_sz(a)-1)
#define arr_end(a)                ((a)+arr_sz(a))
#define arr_foreach(a, type, it)  for (type *it=a; it!=arr_end(a); ++it)

#define arr_reserve(a, n) ((a)=_arr_reserve(a, n, sizeof(*a)))
#define arr_grow(a) arr_reserve(a, max(arr_cap(a)*3/2, arr_cap(a)+1))
#define arr_append_null(a) \
	(arr_sz(a) == arr_cap(a) ? arr_grow(a) : 0, ++arr_sz(a), arr_last(a))
#define arr_append(a, e) \
	UNUSED(arr_append_null(a)), memcpy(arr_last(a), e, sizeof(*a))
#define arr_insert_null(a, i) \
	(UNUSED(arr_append_null(a)), _arr_insert_null(a, i, sizeof(*a)))
#define arr_insert(a, i, e) memcpy(arr_insert_null(a, i), e, sizeof(*a))

#define arr_remove(a, e)   _arr_remove(a, e, sizeof(*a))
#define arr_pop(a)         arr_remove(a, arr_last(a))
#define arr_clear(a)       arr_sz(a) = 0

#define arr_reverse(a)          _arr_reverse(a, sizeof(*a))
#define arr_qsort(a, cmp)       qsort(a, arr_sz(a), sizeof(*a), cmp)
#define arr_bsearch(a, e, cmp)  bsearch(e, a, arr_sz(a), sizeof(*a), cmp)
#define arr_contains(a, e)      arr_contains_ex(a, e, memcmp)
#define arr_index(a, e, i)      arr_index_ex(a, e, i, memcmp)
#define arr_upper(a, e, cmp)    _arr_upper(a, e, sizeof(*a), cmp)

#define arr_contains_ex(a, e, cmp) \
	(_arr_contains(a, e, sizeof(*a), cmp)!=-1)
#define arr_index_ex(a, e, i, cmp) \
	((i=_arr_contains(a, e, sizeof(*a), cmp))!=-1)



void *_arr_create();
void *_arr_reserve(void *a, size_t nmemb, size_t sz);
void *_arr_insert_null(void *a, u32 idx, size_t sz);
void _arr_remove(void *a, void *e, size_t sz);
void _arr_reverse(void *a, size_t sz);
u32 _arr_contains(void *a, const void *elem, size_t sz,
                  int(*cmp)(const void *, const void *, size_t));
void *_arr_upper(void *a, const void *elem, size_t sz,
                 int(*cmp)(const void *, const void*));

#endif
