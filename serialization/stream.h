#ifndef VIOLET_STREAM_H
#define VIOLET_STREAM_H

#include <stdarg.h>
#include <stdio.h>

#include "violet/core/types.h"

typedef struct array array;

typedef enum
{
	FILE_STREAM,
	MEMORY_STREAM
} stream_type;

typedef struct istream
{
	stream_type type;
	union
	{
		FILE * file;
		void * buf;
	};
} istream;

b8 stream_read(istream * s, void * buf, u32 sz);


typedef struct ostream
{
	stream_type type;
	union
	{
		FILE * file;
		array * arr;
	};
} ostream;

void stream_write(ostream * s, const char * format, ...);
void stream_vwrite(ostream * s, const char * format, va_list args);

#endif

