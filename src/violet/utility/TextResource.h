#ifndef VIOLET_TextResource_H
#define VIOLET_TextResource_H

#include "violet/utility/unique_val.h"

#include <fstream>
#include <sstream>

namespace Violet
{
	class TextResource
	{
	public:

		virtual unique_val<std::istream> read() const = 0;
		virtual std::string const & getName() const = 0;
	};

	class StringResource : public TextResource
	{
	public:

		StringResource(std::string name, std::string text) :
			TextResource(),
			m_name(std::move(name)),
			m_text(std::move(text))
		{
		}

		virtual unique_val<std::istream> read() const override
		{
			return make_unique_val<std::istringstream>(m_text);
		}

		virtual std::string const & getName() const override
		{
			return m_name;
		}

	private:

		const std::string m_name;
		const std::string m_text;
	};

	class FileResource : public TextResource
	{
	public:

		FileResource(std::string filename) :
			TextResource(),
			m_filename(std::move(filename))
		{
		}

		virtual unique_val<std::istream> read() const override
		{
			return make_unique_val<std::ifstream>(m_filename);
		}

		virtual std::string const & getName() const override
		{
			return m_filename;
		}

	private:

		const std::string m_filename;
	};
}

#endif