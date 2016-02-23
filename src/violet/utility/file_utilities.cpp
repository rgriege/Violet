// ============================================================================

#include <fstream>
#include <iterator>

#include "violet/core/defines.h"
#include "violet/utility/file_utilities.h"

using namespace vlt;

// ============================================================================

bool File_Utilities::exists(const char * const fileName)
{
	std::ifstream stream(fileName, std::ios_base::binary);
	return !stream.fail();
}

// ----------------------------------------------------------------------------

bool File_Utilities::copy(const char * src, const char * dst)
{
	std::ifstream srcStream(src, std::ios_base::binary);
	if (!srcStream)
		return false;

	srcStream >> std::noskipws;

	std::ofstream dstStream(dst, std::ios_base::binary | std::ios_base::trunc);
	if (!dstStream)
		return false;

	std::istream_iterator<u8> srcBegin(srcStream);
	std::istream_iterator<u8> srcEnd;
	std::ostream_iterator<u8> dstBegin(dstStream);

	std::copy(srcBegin, srcEnd, dstBegin);
	return true;
}

// ============================================================================
