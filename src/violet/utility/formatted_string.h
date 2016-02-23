#ifndef VIOLET_FORMATTED_STRING_H
#define VIOLET_FORMATTED_STRING_H

#include <cstdarg>

#include "violet/core/types.h"

namespace vlt
{
	template <u32 BufferSize>
	struct Formatted_String
	{
		char buffer[BufferSize];

		const char * sprintf(const char * format, ...);
	};

	template <u32 BufferSize>
	const char * Formatted_String<BufferSize>::sprintf(const char * format, ...)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, BufferSize, format, args);
		va_end(args);
		return buffer;
	}
}

#endif
