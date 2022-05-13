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
#define ENFORCE_ARRAY_OF_ARRAY(a)  ((void)(**(a)), (array(array(void)))(a))

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

#define array_copy(dst, src)       ((dst)=array__copy(dst, src, array__esz(src) \
                                                      MEMCALL_LOCATION))
#define array_memcpy(dst, src)     memcpy((dst), (src), array_sz(src) * array__esz(src))

#define array_from_end(a, i)       ((a)[array_sz(a)-i])
#define array_first(a)             ((a)[0])
#define array_last(a)              array_from_end(a, 1)
#define array_end(a)               ((a)+array_sz(a))

#define array_foreach(a, type, it) for (type *it = (a); it != array_end(a); ++it)
#define array_iterate(a, it, n)    for (array_size_t it = 0, n = array_sz(a); \
                                        it < n; ++it)

#define array_reserve(a, n)        ((a)=array__reserve(a, n, array__esz(a) \
                                                       MEMCALL_LOCATION))
#define array_reserve_addl(a, n)   array_reserve(a, array_sz(a) + n)
#define array_set_sz(a, n)         (array_reserve(a, n), array_sz(a) = n)
#define array_extend_sz(a, n)      (array_reserve_addl(a, n), array_sz(a) += n)
#define array_grow(a, n)           ((a)=array__grow(a, n, array__esz(a) \
                                                    MEMCALL_LOCATION))
#define array_append_null(a)       ((a)=array__append_null(a, array__esz(a) \
                                                           MEMCALL_LOCATION), \
                                    (a)+array_sz(a) - 1)
#define array_append(a, e)         (*array_append_null(a) = e)
#define array_appendn(a, p, n)     (assert((a) != (p)), \
                                    (a)=array__appendn(a, p, n, array__esz(a) \
                                                       MEMCALL_LOCATION))
#define array_insert_null(a, i)    ((a)=array__insert_null(a, i, array__esz(a) \
                                                           MEMCALL_LOCATION), \
                                    (a)+(i))
#define array_insert(a, i, e)      (*array_insert_null(a, i) = e)
#define array_insert_fast(a, i, e) ((a)=array__append_null(a, array__esz(a) \
                                                           MEMCALL_LOCATION), \
                                    array_last(a) = (a)[(i)], (a)[(i)] = (e))
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

#define array_array_destroy(a)     array__array_destroy(ENFORCE_ARRAY_OF_ARRAY(a))
#define array_array_clear(a)       array__array_clear(ENFORCE_ARRAY_OF_ARRAY(a))
	
#define array_array_copy(dst, src) (array_set_sz(dst, array_sz(src)), \
                                    array__array_copy(ENFORCE_ARRAY_OF_ARRAY(dst), \
                                                      ENFORCE_ARRAY_OF_ARRAY(src), \
                                                      array__esz(dst[0]) \
                                    MEMCALL_LOCATION))

/* Array To Pointer + Number */
#define A2PN(a)                    (a), array_sz(a)


ARRDEF void *array__create(array_size_t cap, size_t sz, allocator_t *a
                           MEMCALL_ARGS);
ARRDEF void *array__reserve(void *a, array_size_t nmemb, size_t sz
                            MEMCALL_ARGS);
ARRDEF void *array__copy(void *dst, const void *src, size_t sz  MEMCALL_ARGS);
ARRDEF void *array__grow(void *a, array_size_t nmemb, size_t sz  MEMCALL_ARGS);
ARRDEF void *array__append_null(void *a, size_t sz  MEMCALL_ARGS);
ARRDEF void *array__appendn(void *a, const void *p, array_size_t n, size_t sz
                            MEMCALL_ARGS);
ARRDEF void *array__insert_null(void *a, array_size_t idx, size_t sz
                                MEMCALL_ARGS);
ARRDEF void array__remove(void *a, array_size_t idx, array_size_t n, size_t sz);
ARRDEF void array__remove_fast(void *a, array_size_t idx, size_t sz);
ARRDEF void *array__find(void *a, const void *userp, size_t sz,
                         int(*cmp)(const void *, const void *));
ARRDEF array_size_t array__index(void *a, const void *userp, size_t sz,
                                 int(*cmp)(const void *, const void *));
ARRDEF void *array__upper(void *a, const void *elem, size_t sz,
                          int(*cmp)(const void *, const void*));
ARRDEF void  array__array_destroy(array(array(void)) a);
ARRDEF void  array__array_clear(array(array(void)) a);
ARRDEF void  array__array_copy(array(array(void)) dst, array(array(void)) src,
                               size_t sz MEMCALL_ARGS);

#endif

#ifdef ARRAY_IMPLEMENTATION

typedef char *arr_bytep;

ARRDEF void *array__create(array_size_t cap, size_t sz, allocator_t *a
                           MEMCALL_ARGS)
{
	array__head *head = a->malloc_(sizeof(array__head) + cap * sz, a  MEMCALL_VARS);
	if (!head) { fatal("array__create: oom"); return NULL; }
	head->sz = 0;
	head->cap = cap;
	head->allocator = a;
	return head + 1;
}

ARRDEF void *array__reserve(void *array, array_size_t nmemb, size_t sz
                            MEMCALL_ARGS)
{
	array__head *head = array__get_head(array);
	if (nmemb > head->cap) {
		head = head->allocator->realloc_(head, sizeof(array__head) + nmemb * sz,
		                                 head->allocator  MEMCALL_VARS);
		if (!head) { fatal("array__reserve: oom"); return NULL; }
		head->cap = nmemb;
		return head + 1;
	}
	return array;
}

ARRDEF void *array__copy(void *dst, const void *src, size_t sz  MEMCALL_ARGS)
{
	dst = array__reserve(dst, array_sz(src), sz  MEMCALL_VARS);
	array_sz(dst) = array_sz(src);
	memcpy(dst, src, array_sz(src) * sz);
	return dst;
}

ARRDEF void *array__grow(void *a, array_size_t nmemb, size_t sz  MEMCALL_ARGS)
{
	const array_size_t req = array_sz(a) + nmemb;
	return req > array_cap(a)
	     ? array__reserve(a, max(req, array_cap(a)*3/2), sz  MEMCALL_VARS)
	     : a;
}

ARRDEF void *array__append_null(void *a, size_t sz  MEMCALL_ARGS)
{
	a = array__grow(a, 1, sz  MEMCALL_VARS);
	++array_sz(a);
	return a;
}

ARRDEF void *array__appendn(void *a, const void *p, array_size_t n, size_t sz
                            MEMCALL_ARGS)
{
	a = array__grow(a, n, sz  MEMCALL_VARS);
	memcpy(((char*)a)+array_sz(a)*sz, p, n*sz);
	array_sz(a) += n;
	return a;
}

ARRDEF void *array__insert_null(void *a, array_size_t idx, size_t sz
                                MEMCALL_ARGS)
{
	assert(idx <= array_sz(a));
	a = array__append_null(a, sz  MEMCALL_VARS);
	for (arr_bytep p = (arr_bytep)a+(array_sz(a)-1)*sz,
	               end = (arr_bytep)a+idx*sz;
	     p != end; p -= sz)
		memcpy(p, p - sz, sz);
	return a;
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

ARRDEF void array__array_copy(array(array(void)) dst, array(array(void)) src,
                              size_t sz MEMCALL_ARGS)
{
	array_iterate(src, i, n)
		dst[i] = array__copy(array__create(array_sz(src[i]), sz,
		                                   array__allocator(dst) MEMCALL_VARS),
		                     src[i], sz MEMCALL_VARS);
}

#undef ARRAY_IMPLEMENTATION
#endif // ARRAY_IMPLEMENTATION
