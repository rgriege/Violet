#ifndef VIOLET_FILE_UTILITIES_H
#define VIOLET_FILE_UTILITIES_H

namespace vlt
{
	struct File_Utilities
	{
		static bool exists(const char * fileName);
		static bool copy(const char * src, const char * dst);
	};
}

#endif
