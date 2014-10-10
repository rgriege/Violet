#ifndef WINDOW_H
#define WINDOW_H

#include "Config.h"

#include <memory>
#include <string>

namespace Violet
{
	class VIOLET_GRAPHICS_API Window
	{
	public:

		static std::unique_ptr<Window> create(std::string const & title);

	public:

		~Window();

	private:

		Window(int handle);

	private:

		int const m_handle;
	};
}

#endif