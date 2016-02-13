#ifndef VIOLET_FILE_UTILITIES_H
#define VIOLET_FILE_UTILITIES_H

namespace vlt
{
	struct file_utilities
	{
	public:

		static bool exists(const char * fileName);
		static bool copy(const char * src, const char * dst);
	};
}

#endif