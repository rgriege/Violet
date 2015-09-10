#ifndef VIOLET_Tag_H
#define VIOLET_Tag_H

#include "violet/Defines.h"

namespace Violet
{
	class VIOLET_API Tag
	{
	public:

		Tag(char a, char b, char c, char d);

	public:

		const char * asString() const;

		bool operator<(const Tag other) const;
		bool operator==(const Tag other) const;
		bool operator!=(const Tag other) const;

	private:

		char m_data[4];
	};
}

#endif