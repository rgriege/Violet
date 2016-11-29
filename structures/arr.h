#ifndef VIOLET_ARR_H
#define VIOLET_ARR_H

#include <stdlib.h>

#include "violet/core/macros.h"
#include "violet/core/types.h"
#include "violet/math/common.h"
#include "violet/utility/mem.h"


#define DEFINE_ARRAY(type) typedef struct type##_a type##_a;

#define DECLARE_ARRAY(type) \
	typedef struct type##_a \
	{ \
		type *d; \
		u32 sz, cap; \
	} type##_a;

#define DECLARE_ARRAY_NTYPEDEF(type) \
	typedef struct type##_a \
	{ \
		struct type *d; \
		u32 sz, cap; \
	} type##_a;



#define arr_init(a)     ((a).d=0, (a).sz=(a).cap=0)
#define arr_destroy(a)  (free((a).d), arr_init(a))

#define _arr_dsz(a) sizeof(*(a).d)

#define arr_cp(dst, src) \
	arr_reserve(dst, (src).sz); \
	(dst).sz=(src).sz; \
	memcpy((dst).d, (src).d, (src).sz*_arr_dsz(src))

#define arr_last(a)               ((a).d+(a).sz-1)
#define arr_end(a)                ((a).d+(a).sz)
#define arr_foreach(a, type, it) \
	for (type *it=(a).d; it!=arr_end(a); ++it)

#define arr_reserve(a, n)  _arr_reserve(&(a), n, _arr_dsz(a))
#define arr_grow(a)        arr_reserve(a, max((a).cap*3/2, (a).cap+1))
#define arr_append_null(a) \
	((a).sz == (a).cap ? arr_grow(a) : 0, ++(a).sz, arr_last(a))
#define arr_append(a, e) \
	UNUSED(arr_append_null(a)), memcpy(arr_last(a), e, _arr_dsz(a))
#define arr_insert_null(a, i) \
	(UNUSED(arr_append_null(a)), _arr_insert_null(&(a), i, _arr_dsz(a)))
#define arr_insert(a, i, e) \
	memcpy(arr_insert_null(a, i), e, _arr_dsz(a))

#define arr_remove(a, e)   _arr_remove(&(a), e, _arr_dsz(a))
#define arr_pop(a)         arr_remove(a, arr_last(a))
#define arr_clear(a)       (a).sz=0

#define arr_reverse(a)         _arr_reverse(&(a), _arr_dsz(a))
#define arr_qsort(a, cmp)      qsort((a).d, (a).sz, _arr_dsz(a), cmp)
#define arr_bsearch(a, e, cmp) bsearch(e, (a).d, (a).sz, _arr_dsz(a), cmp)
#define arr_contains(a, e)     arr_contains(a, e, memcmp)
#define arr_index(a, e, i)     arr_index_ex(a, e, i, memcmp)
#define arr_upper(a, e, cmp)   _arr_upper(&(a), e, cmp, _arr_dsz(a))

#define arr_contains_ex(a, e, cmp) \
	(_arr_contains(&(a), e, _arr_dsz(a), cmp)!=-1)
#define arr_index_ex(a, e, i, cmp) \
	((i=_arr_contains(&(a), e, _arr_dsz(a), cmp))!=-1)


void _arr_reserve(void *a, size_t nmemb, size_t sz);
void *_arr_insert_null(void *a, u32 idx, size_t sz);
void _arr_remove(void *a, void *e, size_t sz);
void _arr_reverse(void *a, size_t sz);
u32 _arr_contains(void *a, const void *elem, size_t sz,
                  int(*cmp)(const void *, const void *, size_t));
void *_arr_upper(void *a, const void *elem, size_t sz,
                 int(*cmp)(const void *, const void*));

#endif
