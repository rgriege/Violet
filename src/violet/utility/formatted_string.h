#ifndef VIOLET_FORMATTED_STRING_H
#define VIOLET_FORMATTED_STRING_H

#include "violet/core/types.h"

#include <cstdarg>

namespace vlt
{
	template <u32 bufferSize>
	struct formatted_string
	{
	public:

		const char * sprintf(const char * format, ...);

	private:

		char m_buffer[bufferSize];
	};

	template <u32 bufferSize>
	const char * formatted_string<bufferSize>::sprintf(const char * format, ...)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(m_buffer, bufferSize, format, args);
		va_end(args);
		return m_buffer;
	}
}

#endif