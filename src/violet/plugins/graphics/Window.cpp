#include "violet\plugins\graphics\Window.h"

#include <GL\freeglut.h>

using namespace Violet;

std::unique_ptr<Window> Window::create(std::string const & title)
{
	int const handle = glutCreateWindow(title.c_str());
	if (handle == 0)
		return nullptr;
	return std::unique_ptr<Window>(new Window(handle));
}

Window::Window(int handle) :
	m_handle(handle)
{
}

Window::~Window()
{
	glutDestroyWindow(m_handle);
}