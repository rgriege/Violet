#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "violet/math/common.h"
#include "violet/structures/arr.h"

typedef struct arr
{
	byte *d;
	u32 sz, cap;
} arr;

void _arr_reserve(void *_a, size_t nmemb, size_t sz)
{
	arr *a = _a;
	if (nmemb > a->cap)
	{
		a->d = realloc(a->d, nmemb*sz);
		a->cap = nmemb;
	}
}

void *_arr_insert_null(void *_a, u32 idx, size_t sz)
{
	arr *a = _a;
	assert(idx <= a->sz);
	for (byte *p = a->d+(a->sz-1)*sz, *end=a->d+idx*sz; p!=end; p-=sz)
		memcpy(p, p-sz, sz);
	return a+idx*sz;
}

void _arr_remove(void *_a, void *_e, size_t sz)
{
	arr *a = _a;
	byte *e = _e;
	assert(e >= a->d && e < a->d+a->sz*sz && (e-a->d)%sz == 0);
	for (byte *p = e+sz, *end=a->d+a->sz*sz; p!=end; p+=sz)
		memcpy(p-sz, p, sz);
	--a->sz;
}

void _arr_reverse(void *_a, size_t sz)
{
	arr *a = _a;
	byte *scratch = malloc(sz);
	for (u32 i = 0, n = a->sz/2; i < n; ++i)
	{
		byte *l = a->d+i*sz;
		byte *r = a->d+(a->sz-1-i)*sz;
		memcpy(scratch, r, sz);
		memcpy(r, l, sz);
		memcpy(l, scratch, sz);
	}
	free(scratch);
}

u32 _arr_contains(void *_a, const void *elem, size_t sz,
                  int(*cmp)(const void *, const void *, size_t))
{
	arr *a = _a;
	for (byte *p=a->d, *end=a->d+a->sz*sz; p!=end; p+=sz)
		if (cmp(elem, p, sz) == 0)
			return (p-a->d)/sz;
	return -1;
}

void *_arr_upper(void *_a, const void *elem, size_t sz,
                 int(*cmp)(const void *, const void*))
{
	arr *a = _a;
	u32 left = 0, right = a->sz, mid = a->sz/2;
	void *res = NULL;
	while (left != right)
	{
		void *p = a->d+mid*sz;
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

