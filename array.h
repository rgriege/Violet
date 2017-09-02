#ifndef VIOLET_ARRAY_H
#define VIOLET_ARRAY_H

#ifndef array_size_t
#define array_size_t size_t
#endif

#ifdef ARRAY_STATIC
#define ARRDEF static
#else
#define ARRDEF
#endif

#if defined ARRAY_STATIC && !defined ARRAY_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#endif

#include <stdlib.h>

typedef struct array__head
{
	array_size_t sz, cap;
} array__head;

#define array(type) type*

#define array__get_head(a)         (((array__head*)(a)) - 1)
#define array_create()             array__create(0, 0)
#define array_init(a, cap)         (a)=array__create(cap, sizeof(*a))
#define array_destroy(a)           free(array__get_head(a))

#define array__esz(a)              (sizeof(*(a)))
#define array_sz(a)                (array__get_head(a)->sz)
#define array_cap(a)               (array__get_head(a)->cap)
#define array_empty(a)             (array_sz(a) == 0)

#define array_copy(dst, src)       ((dst)=array__copy(dst, src, array__esz(src)))

#define array_from_end(a, i)       ((a)[array_sz(a)-i])
#define array_first(a)             ((a)[0])
#define array_last(a)              array_from_end(a, 1)
#define array_end(a)               ((a)+array_sz(a))

#define array_foreach(a, type, it) for (type *it = (a); it != array_end(a); ++it)
#define array_iterate(a, it, n)    for (array_size_t it = 0, n = array_sz(a); \
                                        it < n; ++it)

#define array_reserve(a, n)        ((a)=array__reserve(a, n, array__esz(a)))
#define array_set_sz(a, n)         (array_reserve(a, n), array_sz(a) = n)
#define array_append_null(a)       ((a)=array__append_null(a, array__esz(a)), \
                                    (a)+array_sz(a) - 1)
#define array_append(a, e)         (*array_append_null(a) = e)
#define array_insert_null(a, i)    ((a)=array__insert_null(a, i, array__esz(a)), \
                                                           (a)+(i))
#define array_insert(a, i, e)      (*array_insert_null(a, i) = e)
#define array_insert_fast(a, i, e) ((a)=array__append_null(a, array__esz(a)), \
                                    array_last(a) = (a)[i], (a)[i] = e)
#define array_remove(a, i)         array__remove(a, i, array__esz(a))
#define array_remove_fast(a, i)    array__remove_fast(a, i, array__esz(a))
#define array_pop(a)               (--array_sz(a))
#define array_clear(a)             (array_sz(a) = 0)

#define array_reverse(a)           array__reverse(a, array__esz(a))
#define array_qsort(a, cmp)        qsort(a, array_sz(a), array__esz(a), cmp)
#define array_bsearch(a, e, cmp)   bsearch(&(e), a, array_sz(a), \
                                           array__esz(a), cmp)
#define array_find(a, p, cmp)      array__find(a, p, array__esz(a), cmp)
#define array_index(a, p, cmp)     array__index(a, p, array__esz(a), cmp)
#define array_upper(a, e, cmp)     array__upper(a, &(e), array__esz(a), cmp)

#define A2PN(a)                    (a), array_sz(a)


ARRDEF void *array__create(array_size_t cap, size_t sz);
ARRDEF void *array__reserve(void *a, array_size_t nmemb, size_t sz);
ARRDEF void *array__copy(void *dst, const void *src, size_t sz);
ARRDEF void *array__append_null(void *a, size_t sz);
ARRDEF void *array__insert_null(void *a, array_size_t idx, size_t sz);
ARRDEF void array__remove(void *a, array_size_t idx, size_t sz);
ARRDEF void array__remove_fast(void *a, array_size_t idx, size_t sz);
ARRDEF void array__reverse(void *a, size_t sz);
ARRDEF void *array__find(void *a, const void *userp, size_t sz,
                         int(*cmp)(const void *, const void *));
ARRDEF array_size_t array__index(void *a, const void *userp, size_t sz,
                                 int(*cmp)(const void *, const void *));
ARRDEF void *array__upper(void *a, const void *elem, size_t sz,
                          int(*cmp)(const void *, const void*));

#endif

#ifdef ARRAY_IMPLEMENTATION

#include <assert.h>
#include <string.h>

typedef char *arr_bytep;

ARRDEF void *array__create(array_size_t cap, size_t sz)
{
	array__head *head = malloc(sizeof(array__head) + cap * sz);
	head->sz = 0;
	head->cap = cap;
	return head + 1;
}

ARRDEF void *array__reserve(void *array, array_size_t nmemb, size_t sz)
{
	array__head *head = ((array__head*)array) - 1;
	if (nmemb > head->cap) {
		head = realloc(head, sizeof(array__head) + nmemb * sz);
		head->cap = nmemb;
		return head + 1;
	}
	return array;
}

ARRDEF void *array__copy(void *dst, const void *src, size_t sz)
{
	dst = array__reserve(dst, array_sz(src), sz);
	array_sz(dst) = array_sz(src);
	memcpy(dst, src, array_sz(src) * sz);
	return dst;
}

ARRDEF void *array__append_null(void *a, size_t sz)
{
	if (array_sz(a) == array_cap(a))
		a = array__reserve(a, array_cap(a)*3/2+1, sz);
	++array_sz(a);
	return a;
}

ARRDEF void *array__insert_null(void *a, array_size_t idx, size_t sz)
{
	a = array__append_null(a, sz);
	assert(idx <= array_sz(a));
	for (arr_bytep p = (arr_bytep)a+(array_sz(a)-1)*sz,
	               end = (arr_bytep)a+idx*sz;
	     p != end; p -= sz)
		memcpy(p, p - sz, sz);
	return a;
}

ARRDEF void array__remove(void *a, array_size_t idx, size_t sz)
{
	assert(idx < array_sz(a));
	for (arr_bytep p = (arr_bytep)a+(idx+1)*sz, end = (arr_bytep)a+array_sz(a)*sz;
	     p != end; p += sz)
		memcpy(p - sz, p, sz);
	--array_sz(a);
}

ARRDEF void array__remove_fast(void *a, array_size_t idx, size_t sz)
{
	assert(array_sz(a) > 0);
	if (idx != array_sz(a) - 1)
		memcpy((arr_bytep)a + idx * sz, (arr_bytep)a + (array_sz(a) - 1) * sz, sz);
	--array_sz(a);
}

ARRDEF void array__reverse(void *a, size_t sz)
{
	arr_bytep scratch = malloc(sz);
	for (array_size_t i = 0, n = array_sz(a)/2; i < n; ++i) {
		arr_bytep l = (arr_bytep)a+i*sz;
		arr_bytep r = (arr_bytep)a+(array_sz(a)-1-i)*sz;
		memcpy(scratch, r, sz);
		memcpy(r, l, sz);
		memcpy(l, scratch, sz);
	}
	free(scratch);
}

ARRDEF void *array__find(void *a, const void *userp, size_t sz,
                         int(*cmp)(const void *, const void *))
{
	for (arr_bytep p=a, end=(arr_bytep)a+array_sz(a)*sz; p!=end; p+=sz)
		if (cmp(p, userp) == 0)
			return p;
	return NULL;
}

ARRDEF array_size_t array__index(void *a, const void *userp, size_t sz,
                                 int(*cmp)(const void *, const void *))
{
	for (arr_bytep p=a, end=(arr_bytep)a+array_sz(a)*sz; p!=end; p+=sz)
		if (cmp(p, userp) == 0)
			return (p-(arr_bytep)a)/sz;
	return ~0;
}

ARRDEF void *array__upper(void *a, const void *elem, size_t sz,
                          int(*cmp)(const void *, const void*))
{
	size_t left = 0, right = array_sz(a), mid = right/2;
	void *res = NULL;
	while (left != right)
	{
		void *p = (arr_bytep)a+mid*sz;
		if (cmp(elem, p) < 0)
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

#undef ARRAY_IMPLEMENTATION
#endif // ARRAY_IMPLEMENTATION
