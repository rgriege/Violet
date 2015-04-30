#include "engine/utility/FileUtilities.h"

#include "engine/Defines.h"

#include <fstream>
#include <iostream>
#include <iterator>

using namespace Violet;

bool FileUtilities::copy(const char * src, const char * dst)
{
	std::ifstream srcStream(src, std::ios_base::binary);
	if (!srcStream)
		return false;

	srcStream >> std::noskipws;

	std::ofstream dstStream(dst, std::ios_base::binary | std::ios_base::trunc);
	if (!dstStream)
		return false;

	std::istream_iterator<uint8> srcBegin(srcStream);
	std::istream_iterator<uint8> srcEnd;
	std::ostream_iterator<uint8> dstBegin(dstStream);

	std::copy(srcBegin, srcEnd, dstBegin);
	return true;
}