#ifndef FORMATTED_STRING_H
#define FORMATTED_STRING_H

#include "violet/Defines.h"

#include <cstdarg>

namespace Violet
{
	template <uint32 bufferSize>
	class FormattedString
	{
	public:

		const char * sprintf(const char * format, ...);

	private:

		char m_buffer[bufferSize];
	};

	template <uint32 bufferSize>
	const char * FormattedString<bufferSize>::sprintf(const char * format, ...)
	{
		va_list args;
		va_start(args, format);
		vsnprintf(m_buffer, bufferSize, format, args);
		va_end(args);
		return m_buffer;
	}
}

#endif