#ifndef VIOLET_TEXT_RESOURCE_H
#define VIOLET_TEXT_RESOURCE_H

#include <fstream>
#include <sstream>

#include "violet/utility/unique_val.h"

namespace vlt
{
	struct Text_Resource
	{
		std::string name;

		Text_Resource(std::string name) :
			name(std::move(name))
		{
		}

		virtual unique_val<std::istream> read() const = 0;
	};

	struct String_Resource final : public Text_Resource
	{
		const std::string text;

		String_Resource(std::string name, std::string text) :
			Text_Resource(name),
			text(std::move(text))
		{
		}

		virtual unique_val<std::istream> read() const override
		{
			return make_unique_val<std::istringstream>(text);
		}
	};

	struct File_Resource final : public Text_Resource
	{
		File_Resource(std::string filename) :
			Text_Resource(filename)
		{
		}

		virtual unique_val<std::istream> read() const override
		{
			return make_unique_val<std::ifstream>(name);
		}
	};
}

#endif
