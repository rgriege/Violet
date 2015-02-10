#include "violet/core/utility/StringUtilities.h"

#include <cstring>
#include <algorithm>

using namespace Violet;

std::string StringUtilities::left(std::string const & str, char delimiter)
{
	return str.substr(0, str.find(delimiter));
}

std::string StringUtilities::right(std::string const & str, char delimiter)
{
	return str.substr(std::min(str.find(delimiter), str.size()), std::string::npos);
}

const char * StringUtilities::right(const char * str, char delimiter)
{
	return strchr(str, delimiter) + 1;
}

void StringUtilities::replace(std::string & haystack, std::string const & needle, std::string const & replacement)
{
	size_t pos = haystack.find(needle.c_str());
	while (pos != std::string::npos)
	{
		haystack.replace(pos, needle.size(), replacement.c_str());
		pos = haystack.find(needle.c_str());
	}
}