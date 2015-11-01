#ifndef VIOLET_FileUtilities_H
#define VIOLET_FileUtilities_H

namespace Violet
{
	class FileUtilities
	{
	public:

		static bool exists(const char * fileName);
		static bool copy(const char * src, const char * dst);
	};
}

#endif