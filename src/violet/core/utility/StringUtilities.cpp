#include "violet/core/utility/StringUtilities.h"

#include <cstring>

using namespace Violet;

const char * StringUtilities::right(const char * str, char delimiter)
{
	return strchr(str, delimiter) + 1;
}