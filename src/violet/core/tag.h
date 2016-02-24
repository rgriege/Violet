#ifndef VIOLET_TAG_H
#define VIOLET_TAG_H

#include "violet/core/defines.h"

#include <string>

namespace vlt
{
	struct VIOLET_API Tag
	{
		union
		{
			char characters[4];
			u32 value;
		};

		Tag() = default;
		Tag(char a, char b, char c, char d);
		explicit Tag(const char * str);
		Tag(const Tag &) = default;

		std::string as_string() const;

		bool operator<(const Tag rhs) const;
		bool operator==(const Tag rhs) const;
		bool operator!=(const Tag rhs) const;
	};
}

namespace std
{
	template <>
	struct hash<vlt::Tag>
	{
		std::size_t operator()(const vlt::Tag tag) const
		{
			return std::hash<u32>()(tag.value);
		}
	};
}

#endif
