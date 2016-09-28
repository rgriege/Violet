#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "violet/serialization/cfg.h"
#include "violet/serialization/stream.h"
#include "violet/serialization/vson.h"
#include "violet/utility/hash.h"

static
b8 _parse_type(const char *type, cfg_val *v)
{
	if (strcmp(type, "str") == 0)
	{
		v->type = CFG_VAL_STR;
		return true;
	}
	return false;
}

b8 cfg_init(cfg *c, const char *filename)
{
	b8 retval = false;
	FILE *fp = fopen(filename, "r");
	if (!fp)
		return false;

	array_map_init(&c->values, sizeof(u32), sizeof(cfg_val));

	istream s = { .type = FILE_STREAM, .file = fp };
	u32 sections;
	if (!vson_read_u32(&s, "n", &sections))
		goto err;

	for (u32 i = 0; i < sections; ++i)
	{
		char section[32];
		u32 entries;
		if (!vson_read_str(&s, "section", section, 32))
			goto err;
		const u32 section_hash = vlt_hash(section);
		if (!vson_read_u32(&s, "n", &entries))
			goto err;
		for (u32 j = 0; j < entries; ++j)
		{
			cfg_val v;
			char type[8];
			char label[32];
			if (!vson_read_str(&s, "label", label, 32))
				goto err;
			if (!vson_read_str(&s, "type", type, 8))
				goto err;
			if (!_parse_type(type, &v))
				goto err;
			switch (v.type)
			{
			case CFG_VAL_STR:
				v.str = malloc(128);
				if (!vson_read_str(&s, "value", v.str, 128))
				{
					free(v.str);
					goto err;
				}
			break;
			}
			const u32 hash = section_hash + vlt_hash(label);
			array_map_insert(&c->values, &hash, &v);
		}
	}

	retval = true;
	goto out;

err:
	cfg_destroy(c);
out:
	return retval;
}

const char *cfg_str(const cfg *c, const char *sec, const char *key)
{
	const u32 hash = vlt_hash(sec) + vlt_hash(key);
	const cfg_val *v = array_map_get(&c->values, &hash);
	return v && v->type == CFG_VAL_STR ? v->str : NULL;
}

static
void _val_destroy(cfg_val *v)
{
	if (v->type == CFG_VAL_STR)
		free(v->str);
}

void cfg_destroy(cfg *c)
{
	array_map_destroy_deep(&c->values, _val_destroy);
}

