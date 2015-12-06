#ifndef VIOLET_Tag_H
#define VIOLET_Tag_H

#include "violet/Defines.h"

#include <array>
#include <string>

namespace Violet
{
	class VIOLET_API Tag
	{
	public:

		Tag(char a, char b, char c, char d);
		explicit Tag(const char * tag);

	public:

		std::string asString() const;

		bool operator<(const Tag other) const;
		bool operator==(const Tag other) const;
		bool operator!=(const Tag other) const;

	private:

		std::array<char, 4> m_data;
	};
}

#endif