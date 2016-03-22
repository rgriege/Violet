#include <assert.h>
#include <stdarg.h>

#include "violet/serialization/stream.h"
#include "violet/structures/array.h"

array g_streams = {0};

void log_init()
{
	array_init(&g_streams, sizeof(ostream *));
}

void log_destroy()
{
	assert(array_empty(&g_streams));
	array_destroy(&g_streams);
}

void log_add_stream(ostream * s)
{
	array_append(&g_streams, &s);
}

b8 log_remove_stream(ostream * s)
{
	for (u32 i = 0, n = array_size(&g_streams); i < n; ++i)
	{
		ostream ** entry = array_get(&g_streams, i);
		if (s == *entry)
		{
			array_remove(&g_streams, i);
			return true;
		}
	}
	return false;
}

void log_write(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	for (u32 i = 0, n = array_size(&g_streams); i < n; ++i)
	{
		va_list args_cp;
		va_copy(args_cp, args);
		ostream ** s = array_get(&g_streams, i);
		stream_vwrite(*s, format, args_cp);
		stream_write(*s, "\n");
		va_end(args_cp);
	}
	va_end(args);
}

