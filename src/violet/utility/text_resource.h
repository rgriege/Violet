#ifndef VIOLET_TEXT_RESOURCE_H
#define VIOLET_TEXT_RESOURCE_H

#include "violet/utility/unique_val.h"

#include <fstream>
#include <sstream>

namespace vlt
{
	struct text_resource
	{
		std::string name;

		text_resource(std::string name) :
			name(std::move(name))
		{
		}

		virtual unique_val<std::istream> read() const = 0;
	};

	struct string_resource final : public text_resource
	{
		const std::string text;

		string_resource(std::string name, std::string text) :
			text_resource(name),
			text(std::move(text))
		{
		}

		virtual unique_val<std::istream> read() const override
		{
			return make_unique_val<std::istringstream>(text);
		}
	};

	struct file_resource final : public text_resource
	{
		file_resource(std::string filename) :
			text_resource(filename)
		{
		}

		virtual unique_val<std::istream> read() const override
		{
			return make_unique_val<std::ifstream>(name);
		}
	};
}

#endif