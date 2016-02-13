#ifndef VIOLET_TAG_H
#define VIOLET_TAG_H

#include "violet/core/defines.h"

#include <array>
#include <string>

namespace vlt
{
	struct VIOLET_API tag
	{
	public:

		tag(char a, char b, char c, char d);
		explicit tag(const char * str);

	public:

		std::string as_string() const;

		bool operator<(const tag rhs) const;
		bool operator==(const tag rhs) const;
		bool operator!=(const tag rhs) const;

	private:

		std::array<char, 4> data;
	};
}

#endif