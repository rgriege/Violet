#ifndef VIOLET_TAG_H
#define VIOLET_TAG_H

#include "violet/core/defines.h"

#include <array>
#include <string>

namespace vlt
{
	struct VIOLET_API tag
	{
		union
		{
			std::array<char, 4> characters;
			u32 value;
		};

		tag() = default;
		tag(char a, char b, char c, char d);
		explicit tag(const char * str);
		tag(const tag &) = default;

		std::string as_string() const;

		bool operator<(const tag rhs) const;
		bool operator==(const tag rhs) const;
		bool operator!=(const tag rhs) const;
	};
}

namespace std
{
	template <>
	struct hash<vlt::tag>
	{
		std::size_t operator()(const vlt::tag tag) const
		{
			return std::hash<u32>()(tag.value);
		}
	};
}

#endif