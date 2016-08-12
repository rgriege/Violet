#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "violet/serialization/stream.h"
#include "violet/structures/array.h"

b8 stream_read(istream * s, void * buf, u32 sz)
{
	assert(sz > 0);
	switch (s->type)
	{
	case MEMORY_STREAM:
		memcpy(buf, s->buf, sz);
		s->buf = (byte*)s->buf + sz;
		return true;

	case FILE_STREAM:
		return fread(buf, 1, sz, s->file) == sz;

	default:
		assert(false);
		return false;
	}
}

void stream_write(ostream * s, const char * format, ...)
{
	va_list args;
	va_start(args, format);
	stream_vwrite(s, format, args);
	va_end(args);
}

void stream_vwrite(ostream * s, const char * format, va_list args)
{
	switch (s->type)
	{
	case MEMORY_STREAM:
	{
		va_list args_sz;
		va_copy(args_sz, args);
		const u32 sz = vsnprintf(NULL, 0, format, args_sz);
		va_end(args_sz);
		array_reserve(s->arr, s->arr->size + sz + 1);
		vsprintf((byte*)s->arr->data + s->arr->size, format, args);
		s->arr->size += sz;
	}
		break;

	case FILE_STREAM:
		vfprintf(s->file, format, args);
		fflush(s->file);
		break;
	}
}

