#include "violet/utility/hash.h"

// djb2 by dan bernstein
u32 vlt_hash(const char *str)
{
	u32 hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

u32 vlt_hashn(const char *str, u32 n)
{
	u32 hash = 5381;
	for (u32 i = 0; i < n; ++i)
		hash = ((hash << 5) + hash) + (int)str[i];
	return hash;
}
