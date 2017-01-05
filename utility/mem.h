#ifndef VIOLET_MEM_H
#define VIOLET_MEM_H

#include <string.h>

static inline void memswp_(void *a, void *b, u32 sz, void *tmp)
{
	memcpy(tmp, a, sz);
	memcpy(a, b, sz);
	memcpy(b, tmp, sz);
}

#define memswp_s(a, b, sz, tmp) \
	assert(sz <= sizeof(tmp)); \
	memswp_(a, b, sz, tmp);

#define memswp(a, b, type) \
	do { type tmp = a; a = b; b = tmp; } while(0)

#endif

