#include "violet/core/window/Window.h"

#include <assert.h>

using namespace Violet;

namespace WindowNamespace
{
	std::unique_ptr<Window>(*ms_creator)(Deserializer & deserializer);
	std::unique_ptr<Window> ms_currentWindow;
}

using namespace WindowNamespace;

void Window::install(std::unique_ptr<Window>(*creator)(Deserializer &))
{
	ms_creator = creator;
}

Window & Window::create(Deserializer & deserializer)
{
	ms_currentWindow = ms_creator(deserializer);
	return Window::getCurrent();
}

Window & Window::getCurrent()
{
	assert(ms_currentWindow != nullptr);
	return *ms_currentWindow;
}