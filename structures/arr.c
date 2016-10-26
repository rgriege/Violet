#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "violet/math/common.h"
#include "violet/structures/arr.h"

void *_arr_create()
{
	u32 *a = malloc(2*sizeof(u32));
	a += 2;
	arr_cap(a) = arr_sz(a) = 0;
	return a;
}

void *_arr_reserve(void *a, size_t nmemb, size_t sz)
{
	if (nmemb > arr_cap(a))
	{
		u32 *p = realloc(arr_raw(a), nmemb*sz+2*sizeof(u32));
		arr_cap(p+2) = nmemb;
		return p+2;
	}
	else
		return a;
}

void *_arr_insert_null(void *_a, u32 idx, size_t sz)
{
	byte *a = _a;
	assert(idx <= arr_sz(a));
	for (byte *p = a+(arr_sz(a)-1)*sz, *end=a+idx*sz; p!=end; p-=sz)
		memcpy(p, p-sz, sz);
	return a+idx*sz;
}

void _arr_remove(void *_a, void *_e, size_t sz)
{
	byte *a = _a;
	byte *e = _e;
	assert(e >= a && e < a+arr_sz(a)*sz && (e-a)%sz == 0);
	for (byte *p = e+sz, *end=a+arr_sz(a)*sz; p!=end; p+=sz)
		memcpy(p-sz, p, sz);
	--arr_sz(a);
}

void _arr_reverse(void *_a, size_t sz)
{
	byte *a = _a;
	byte *scratch = malloc(sz);
	for (u32 i = 0, n = arr_sz(a)/2; i < n; ++i)
	{
		byte *l = a+i*sz;
		byte *r = a+(arr_sz(a)-1-i)*sz;
		memcpy(scratch, r, sz);
		memcpy(r, l, sz);
		memcpy(l, scratch, sz);
	}
	free(scratch);
}

u32 _arr_contains(void *_a, const void *elem, size_t sz,
                  int(*cmp)(const void *, const void *, size_t))
{
	byte *a = _a;
	for (byte *p=a, *end=a+arr_sz(a)*sz; p!=end; p+=sz)
		if (cmp(elem, p, sz) == 0)
			return (p-a)/sz;
	return -1;
}

void *_arr_upper(void *_a, const void *elem, size_t sz,
                 int(*cmp)(const void *, const void*))
{
	byte *a = _a;
	u32 left = 0, right = arr_sz(a), mid = arr_sz(a)/2;
	void *res = NULL;
	while (left != right)
	{
		void *p = a+mid*sz;
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

