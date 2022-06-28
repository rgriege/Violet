#ifndef VIOLET_ARRAY_H
#define VIOLET_ARRAY_H

#define array_size_t u32

#ifdef ARRAY_STATIC
#define ARRDEF static
#else
#define ARRDEF
#endif

#if defined ARRAY_STATIC && !defined ARRAY_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION
#endif

typedef struct array__head
{
	array_size_t sz, cap;
	allocator_t *allocator;
} array__head;

#define array(type) type*
#define CAST_PTR_PTR(a)            ((void)(**(a)), (void**)(a))
#define CAST_PTR_PTR_PTR(a)        ((void)(***(a)), (void***)(a))
#define REF_ARRAY(a)               CAST_PTR_PTR(&(a))
#define REF_ARRAY_ARRAY(a)         CAST_PTR_PTR_PTR(&(a))
#define CAST_ARRAY_ARRAY(a)        CAST_PTR_PTR(a)

#define array__get_head(a)         (((array__head*)(a)) - 1)
#define array__allocator(a)        (array__get_head(a)->allocator)
#define array_create()             array_create_ex(g_allocator)
#define array_create_ex(alc)       array__create(0, 0, alc  MEMCALL_LOCATION)
#define array_init(a, cap)         array_init_ex(a, cap, g_allocator)
#define array_init_ex(a, cap, alc) (a)=array__create(cap, sizeof(*a), alc \
                                                     MEMCALL_LOCATION)
#define array_destroy(a)           afree(array__get_head(a), array__allocator(a))

#define array__esz(a)              (sizeof(*(a)))
#define array_sz(a)                (array__get_head(a)->sz)
#define array_cap(a)               (array__get_head(a)->cap)
#define array_empty(a)             (array_sz(a) == 0)

#define array_copy(dst, src)       (array__copy(REF_ARRAY(dst), src, array__esz(src) \
                                                MEMCALL_LOCATION) ? (dst) : NULL)
#define array_memcpy(dst, src)     memcpy((dst), (src), array_sz(src) * array__esz(src))

#define array_from_end(a, i)       ((a)[array_sz(a)-i])
#define array_first(a)             ((a)[0])
#define array_last(a)              array_from_end(a, 1)
#define array_end(a)               ((a)+array_sz(a))

#define array_foreach(a, type, it) for (type *it = (a); it != array_end(a); ++it)
#define array_iterate(a, it, n)    for (array_size_t it = 0, n = array_sz(a); \
                                        it < n; ++it)

#define array_reserve(a, n)        (array__reserve(REF_ARRAY(a), n, \
                                                   array__esz(a) MEMCALL_LOCATION) \
                                    ? (a) : NULL)
#define array_reserve_addl(a, n)   array_reserve(a, array_sz(a) + n)
#define array_set_sz(a, n)         (array_reserve(a, n) ? array_sz(a) = n : false)
#define array_extend_sz(a, n)      (array_reserve_addl(a, n), array_sz(a) += n)
#define array_grow(a, n)           (array__grow(REF_ARRAY(a), n, array__esz(a) \
                                                MEMCALL_LOCATION) \
                                    ? a : NULL)
#define array_append_null(a)       (array__append_null(REF_ARRAY(a), array__esz(a) \
                                                       MEMCALL_LOCATION) \
                                    ? (a)+array_sz(a) - 1 : NULL)
#define array_append(a, e)         (*array_append_null(a) = e)
#define array_appendn(a, p, n)     (assert((a) != (p)), \
                                    array__appendn(REF_ARRAY(a), p, n, \
                                                   array__esz(a) MEMCALL_LOCATION) \
                                    ? (a) : NULL)
#define array_insert_null(a, i)    (array__insert_null(REF_ARRAY(a), i, \
                                                       array__esz(a) MEMCALL_LOCATION) \
                                    ? (a)+(i) : NULL)
#define array_insert(a, i, e)      ((array__insert_null(REF_ARRAY(a), i, \
                                                       array__esz(a) MEMCALL_LOCATION) \
                                    ? (*((a)+(i)) = e, true) : false))
#define array_insert_fast(a, i, e) (array__append_null(REF_ARRAY(a), \
                                                       array__esz(a) MEMCALL_LOCATION) \
                                    ? ((array_last(a) = (a)[(i)], (a)[(i)] = (e)), true) : false)
#define array_remove(a, i)         array_remove_n(a, i, 1)
#define array_remove_n(a, i, n)    array__remove(a, i, n, array__esz(a))
#define array_remove_fast(a, i)    array__remove_fast(a, i, array__esz(a))
#define array_pop(a)               (--array_sz(a))
#define array_clear(a)             (array_sz(a) = 0)

#define array_reverse(a)           reverse(a, array__esz(a), array_sz(a))
#define array_qsort(a, cmp)        qsort(a, array_sz(a), array__esz(a), cmp)
#define array_ssort(a, cmp)        ssort(a, array_sz(a), array__esz(a), cmp)
#define array_bsearch(a, e, cmp)   bsearch(&(e), a, array_sz(a), \
                                           array__esz(a), cmp)
#define array_find(a, p, cmp)      array__find(a, p, array__esz(a), cmp)
#define array_index(a, p, cmp)     array__index(a, p, array__esz(a), cmp)
#define array_upper(a, e, cmp)     array__upper(a, &(e), array__esz(a), cmp)

#define array_array_destroy(a)     array__array_destroy(CAST_ARRAY_ARRAY(a))
#define array_array_clear(a)       array__array_clear(CAST_ARRAY_ARRAY(a))

#define array_array_copy(dst, src) (array__array_copy(REF_ARRAY_ARRAY(dst), \
                                                      CAST_ARRAY_ARRAY(src), \
                                                      array__esz(dst[0]) \
                                                      MEMCALL_LOCATION) ? dst : NULL)

/* Array To Pointer + Number */
#define A2PN(a)                    (a), array_sz(a)

#define array_reserve_check(a, n)      (array__reserve(CAST_PTR_PTR(a), n, \
                                                       array__esz(*a) \
                                                       MEMCALL_LOCATION))
#define array_reserve_addl_check(a, n) array_reserve_check(a, array_sz(*a) + n)
#define array_set_sz_check(a, n)       (array__set_sz(CAST_PTR_PTR(a), n, \
                                                      array__esz(*a) \
                                                      MEMCALL_LOCATION))

#define fixed_array(type, name, size) struct { \
                                          array__head head; \
                                          type fsa[size]; \
                                      } name##__buffer; array(type) name;

#define fixed_array_init(name)        ((name = name##__buffer.fsa), \
                                      (array__get_head(name)->allocator = g_null_allocator), \
                                      (array__get_head(name)->sz = 0), \
                                      (array__get_head(name)->cap = countof(name##__buffer.fsa)))

CHECK_RETURN ARRDEF void *array__create(array_size_t cap, size_t sz,
                                        allocator_t *a MEMCALL_ARGS);
CHECK_RETURN ARRDEF b32  array__reserve(void **a, array_size_t nmemb,
                                        size_t sz MEMCALL_ARGS);
CHECK_RETURN ARRDEF b32  array__copy(void **dst, const void *src,
                                     size_t sz  MEMCALL_ARGS);
CHECK_RETURN ARRDEF b32  array__grow(void **a, array_size_t nmemb,
                                     size_t sz  MEMCALL_ARGS);
CHECK_RETURN ARRDEF b32  array__append_null(void **a, size_t sz  MEMCALL_ARGS);
CHECK_RETURN ARRDEF b32  array__appendn(void **a, const void *p, array_size_t n,
                                        size_t sz MEMCALL_ARGS);
CHECK_RETURN ARRDEF b32  array__insert_null(void **a, array_size_t idx,
                                            size_t sz MEMCALL_ARGS);
CHECK_RETURN ARRDEF b32  array__set_sz(void **a, array_size_t n,
                                       size_t sz MEMCALL_ARGS);
ARRDEF void              array__remove(void *a, array_size_t idx, array_size_t n,
                                       size_t sz);
ARRDEF void              array__remove_fast(void *a, array_size_t idx,
                                            size_t sz);
ARRDEF void*             array__find(void *a, const void *userp, size_t sz,
                                     int(*cmp)(const void *, const void *));
ARRDEF array_size_t      array__index(void *a, const void *userp, size_t sz,
                                      int(*cmp)(const void *, const void *));
ARRDEF void*             array__upper(void *a, const void *elem, size_t sz,
                                      int(*cmp)(const void *, const void*));
ARRDEF void              array__array_destroy(array(array(void)) a);
ARRDEF void              array__array_clear(array(array(void)) a);
ARRDEF b32               array__array_copy(array(array(void)) *dst,
                                           array(array(void)) src,
                                           size_t sz MEMCALL_ARGS);

#endif

#ifdef ARRAY_IMPLEMENTATION

typedef char *arr_bytep;

CHECK_RETURN ARRDEF void *array__create(array_size_t cap, size_t sz,
                                        allocator_t *a MEMCALL_ARGS)
{
	array__head *head = a->malloc_(sizeof(array__head) + cap * sz, a  MEMCALL_VARS);
	if (!head) { return NULL; }
	head->sz = 0;
	head->cap = cap;
	head->allocator = a;
	return head + 1;
}

CHECK_RETURN ARRDEF b32 array__reserve(void **array, array_size_t nmemb,
                                       size_t sz MEMCALL_ARGS)
{
	array__head *head = array__get_head(*array);
	if (nmemb > head->cap) {
		head = head->allocator->realloc_(head, sizeof(array__head) + nmemb * sz,
		                                 head->allocator  MEMCALL_VARS);
		if (!head)
			return false;
		head->cap = nmemb;
		*array = head + 1;
	}
	return true;
}

CHECK_RETURN ARRDEF b32 array__copy(void **dst, const void *src,
                                    size_t sz  MEMCALL_ARGS)
{
	if (!array__reserve(dst, array_sz(src), sz  MEMCALL_VARS))
		return false;
	array_sz(*dst) = array_sz(src);
	memcpy(*dst, src, array_sz(src) * sz);
	return true;
}

CHECK_RETURN ARRDEF b32 array__grow(void **a, array_size_t nmemb,
                                    size_t sz  MEMCALL_ARGS)
{
	const array_size_t req = array_sz(*a) + nmemb;
	return req > array_cap(*a)
	     ? array__reserve(a, max(req, array_cap(*a)*3/2), sz  MEMCALL_VARS)
	     : true;
}

CHECK_RETURN ARRDEF b32 array__append_null(void **a, size_t sz  MEMCALL_ARGS)
{
	if (!array__grow(a, 1, sz  MEMCALL_VARS))
		return false;
	++array_sz(*a);
	return true;
}

CHECK_RETURN ARRDEF b32 array__appendn(void **a, const void *p, array_size_t n,
                                       size_t sz MEMCALL_ARGS)
{
	if (!array__grow(a, n, sz  MEMCALL_VARS))
		return false;
	memcpy(((char*)*a)+array_sz(*a)*sz, p, n*sz);
	array_sz(*a) += n;
	return true;
}

CHECK_RETURN ARRDEF b32 array__insert_null(void **a, array_size_t idx,
                                           size_t sz MEMCALL_ARGS)
{
	assert(idx <= array_sz(*a));
	if (!array__append_null(a, sz  MEMCALL_VARS))
		return false;
	for (arr_bytep p = ((arr_bytep)*a)+(array_sz(*a)-1)*sz,
	               end = ((arr_bytep)*a)+idx*sz;
	     p != end; p -= sz)
		memcpy(p, p - sz, sz);
	return true;
}

CHECK_RETURN ARRDEF b32  array__set_sz(void **a, array_size_t  n,
                                       size_t sz MEMCALL_ARGS)
{
	if (!array__reserve(a, n, sz MEMCALL_VARS))
		return false;
	array_sz(*(a)) = n;
	return true;
}

ARRDEF void array__remove(void *a, array_size_t idx, array_size_t n, size_t sz)
{
	assert(n > 0);
	assert(idx + n - 1 < array_sz(a));
	for (arr_bytep p = (arr_bytep)a + (idx + n) * sz,
	               end = (arr_bytep)a + array_sz(a) * sz;
	     p != end; p += sz)
		memcpy(p - n * sz, p, sz);
	array_sz(a) -= n;
}

ARRDEF void array__remove_fast(void *a, array_size_t idx, size_t sz)
{
	assert(array_sz(a) > 0);
	if (idx != array_sz(a) - 1)
		memcpy((arr_bytep)a + idx * sz, (arr_bytep)a + (array_sz(a) - 1) * sz, sz);
	--array_sz(a);
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
			return (array_size_t)((p-(arr_bytep)a)/sz);
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

ARRDEF void array__array_destroy(array(array(void)) a)
{
	array_foreach(a, array(void), b)
		array_destroy(*b);
	array_destroy(a);
}

ARRDEF void array__array_clear(array(array(void)) a)
{
	array_foreach(a, array(void), b)
		array_destroy(*b);
	array_clear(a);
}

ARRDEF b32 array__array_copy(array(array(void)) *dst, array(array(void)) src,
                             size_t sz MEMCALL_ARGS)
{
	assert(array_empty(*dst));
	if (!array_set_sz_check(dst, array_sz(src)))
		return false;
	array_iterate(src, i, n) {
		(*dst)[i] = array__create(array_sz(src[i]), sz,
		                          array__allocator(*dst) MEMCALL_VARS);
		if (!(*dst)[i])
			return false;
		if (!array__copy(&(*dst)[i], src[i], sz MEMCALL_VARS))
			return false;
	}
	return true;
}

#undef ARRAY_IMPLEMENTATION
#endif // ARRAY_IMPLEMENTATION
