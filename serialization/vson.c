#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "violet/serialization/vson.h"
#include "violet/serialization/stream.h"
#include "violet/structures/array.h"
#include "violet/utility/log.h"

static const char g_str_end = '\0';

static b8 _vson_read_label(istream * s, const char * label)
{
	b8 retval = false;
	char c = 0;
	while (stream_read(s, &c, sizeof(char)) && isspace((u8)c))
		;
	if (isspace((u8)c))
	{
		log_write("vson: failed reading label %s", label);
		goto out;
	}

	array str;
	array_init(&str, sizeof(char));
	array_append(&str, &c);

	while (stream_read(s, &c, sizeof(char)) && c != ':')
		array_append(&str, &c);
	if (c != ':')
	{
		log_write("vson: missing colon after label %s", label);
		goto err;
	}

	array_append(&str, &g_str_end);
	if (strcmp(str.data, label) != 0)
	{
		log_write("vson: expected %s, got %s", label, str.data);
		goto err;
	}

	retval = stream_read(s, &c, sizeof(char)) && c == ' ';
err:
	array_destroy(&str);
out:
	return retval;
}

static b8 _vson_skip_rest_of_line(istream * s)
{
	char c;
	while (stream_read(s, &c, sizeof(char)) && c != '\n')
		;
	return c == '\n';
}

static b8 _vson_read_rest_of_line(istream * s, array * str)
{
	char c;
	while (stream_read(s, &c, sizeof(char)) && c != '\n')
		array_append(str, &c);
	if (c != '\n')
	{
		_vson_skip_rest_of_line(s);
		return false;
	}
	array_append(str, &g_str_end);
	return true;
}

b8 vson_read_b8(istream * s, const char * label, b8 * val)
{
	char c;
	if (   _vson_read_label(s, label)
		&& stream_read(s, &c, sizeof(char)))
	{
		*val = c != '0' && c != 'f';
		return true;
	}

	_vson_skip_rest_of_line(s);
	return false;
}

b8 vson_read_s32(istream * s, const char * label, s32 * val)
{
	if (!_vson_read_label(s, label))
	{
		_vson_skip_rest_of_line(s);
		return false;
	}

	array str;
	array_init(&str, sizeof(char));
	if (!_vson_read_rest_of_line(s, &str))
	{
		array_destroy(&str);
		return false;
	}

	*val = strtol(str.data, NULL, 10);
	array_destroy(&str);
	return true;
}

b8 vson_read_u32(istream * s, const char * label, u32 * val)
{
	if (!_vson_read_label(s, label))
	{
		_vson_skip_rest_of_line(s);
		return false;
	}

	array str;
	array_init(&str, sizeof(char));
	if (!_vson_read_rest_of_line(s, &str))
	{
		array_destroy(&str);
		return false;
	}

	*val = strtoul(str.data, NULL, 10);
	array_destroy(&str);
	return true;
}

b8 vson_read_r32(istream * s, const char * label, r32 * val)
{
	if (!_vson_read_label(s, label))
	{
		_vson_skip_rest_of_line(s);
		return false;
	}

	array str;
	array_init(&str, sizeof(char));
	if (!_vson_read_rest_of_line(s, &str))
	{
		array_destroy(&str);
		return false;
	}

	*val = strtof(str.data, NULL);
	array_destroy(&str);
	return true;
}

b8 vson_read_str(istream * s, const char * label, char * val, u32 sz)
{
	if (!_vson_read_label(s, label))
	{
		_vson_skip_rest_of_line(s);
		return false;
	}

	char * p = val;
	u32 i = 0;
	while (i < sz && stream_read(s, p, sizeof(char)) && *p != '\n')
	{
		++i;
		++p;
	}
	if (i == sz)
		return true;
	if (*p != '\n')
	{
		*(++p) = '\0';
		return false;
	}

	*p = '\0';
	return true;
}

