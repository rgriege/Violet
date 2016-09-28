#ifndef VIOLET_CFG_H
#define VIOLET_CFG_H

#include "violet/structures/array_map.h"

typedef struct cfg_val
{
	enum
	{
		CFG_VAL_STR
	} type;
	union
	{
		char *str;
	};
} cfg_val;

typedef struct cfg
{
	array_map values;
} cfg;

b8 cfg_init(cfg *c, const char *filename);
const char *cfg_str(const cfg *c, const char *sec, const char *key);
void cfg_destroy(cfg *c);

#endif
