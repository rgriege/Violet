#include "violet/utility/hash.h"

// djb2 by dan bernstein
u32 vlt_hash(const char * str)
{
	u32 hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

