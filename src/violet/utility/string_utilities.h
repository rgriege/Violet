#ifndef VIOLET_STRING_UTILITIES_H
#define VIOLET_STRING_UTILITIES_H

#include <cstring>
#include <string>
#include <vector>

#include "violet/core/defines.h"

namespace vlt
{
	struct VIOLET_API String_Utilities
	{
	public:

		struct Less
		{
			bool operator()(const char * lhs, const char * rhs) const
			{
				return strcmp(lhs, rhs) < 0;
			}
		};

	public:

		static std::string left(std::string const & str, char delimiter);
		static std::string rightOfFirst(std::string const & str, char delimiter);
		static const char * rightOfFirst(const char * str, char delimiter);
		static std::string rightOfLast(std::string const & str, char delimiter);
		static const char * rightOfLast(const char * str, char delimiter);
		static void replace(std::string & haystack, std::string const & needle, std::string const & replacement);
		static void split(const std::string & whole, char delimiter, std::vector<std::string> & pieces);
	};
}

#endif
