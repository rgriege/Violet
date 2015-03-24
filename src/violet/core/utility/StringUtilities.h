#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

#include "violet/core/Defines.h"

#include <string>

namespace Violet
{
	class VIOLET_API StringUtilities
	{
	public:

		struct Less
		{
			bool operator()(const char * lhs, const char * rhs)
			{
				return strcmp(lhs, rhs) < 0;
			}
		};
	public:

		static std::string left(std::string const & str, char delimiter);
		static std::string right(std::string const & str, char delimiter);
		static const char * right(const char * str, char delimiter);
		static void replace(std::string & haystack, std::string const & needle, std::string const & replacement);
	};
}

#endif