#include "violet/utility/string_utilities.h"

#include <cstring>
#include <algorithm>

using namespace vlt;

std::string String_Utilities::left(std::string const & str, char delimiter)
{
	return str.substr(0, str.find(delimiter));
}

std::string String_Utilities::rightOfFirst(std::string const & str, char delimiter)
{
	const auto pos = str.find(delimiter);
	return pos != std::string::npos ? str.substr(pos + 1) : "";
}

const char * String_Utilities::rightOfFirst(const char * str, char delimiter)
{
	return strchr(str, delimiter) + 1;
}

std::string String_Utilities::rightOfLast(std::string const & str, char delimiter)
{
	return str.substr(std::min(str.find_last_of(delimiter) + 1, str.size()), std::string::npos);
}

const char * String_Utilities::rightOfLast(const char * str, char delimiter)
{
	return strrchr(str, delimiter) + 1;
}

void String_Utilities::replace(std::string & haystack, std::string const & needle, std::string const & replacement)
{
	size_t pos = haystack.find(needle.c_str());
	while (pos != std::string::npos)
	{
		haystack.replace(pos, needle.size(), replacement.c_str());
		pos = haystack.find(needle.c_str(), pos + replacement.size());
	}
}

void String_Utilities::split(const std::string & whole, const char delimiter, std::vector<std::string> & pieces)
{
	size_t start = 0;
	size_t end = whole.find(delimiter); 
	for ( ; end != std::string::npos; end = whole.find(delimiter, end + 1))
	{
		pieces.emplace_back(whole.substr(start, end));
		start = end;
	}
	pieces.emplace_back(whole.substr(start, end));
}
