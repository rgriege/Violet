#include "violet/plugins/glut/GlutWindow.h"

#include "violet/core/serialization/Deserializer.h"

#include <GL/freeglut.h>

using namespace Violet;

namespace GlutWindowNamespace
{
	bool ms_quit = false;

	void close();
	void display();
	void onMouse(int button, int state, int x, int y);
	void onKeyboardDown(unsigned char key, int x, int y);
	void onKeyboardUp(unsigned char key, int x, int y);
}

using namespace GlutWindowNamespace;

std::unique_ptr<Window> GlutWindow::create(Deserializer & deserializer)
{
	int argc = 0;
	char * argv = "";
	int const x = deserializer.getInt("x");
	int const y = deserializer.getInt("y");
	int const width = deserializer.getInt("width");
	int const height = deserializer.getInt("height");
	const char * title = deserializer.getString("title");

	glutInit(&argc, &argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(x, y);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	if (glutCreateWindow(title) == 0)
		return nullptr;

	glutCloseFunc(close);
	glutDisplayFunc(display);
	glutMouseFunc(onMouse);
	glutKeyboardFunc(onKeyboardDown);
	glutKeyboardUpFunc(onKeyboardUp);

	return std::unique_ptr<Window>(new GlutWindow(width, height));
}

bool GlutWindow::update()
{
	m_eventQueue.clear();
	glutMainLoopEvent();
	return !ms_quit;
}

void GlutWindow::render()
{
	glutPostRedisplay();
}

bool GlutWindow::getEvent(const EventType type, Event * const event)
{
	for (auto it = m_eventQueue.begin(), end = m_eventQueue.end(); it != end; ++it)
	{
		if ((it->type & type) == it->type)
		{
			*event = *it;
			m_eventQueue.erase(it);
			return true;
		}
	}

	return false;
}

void GlutWindow::addEvent(Event event)
{
	m_eventQueue.push_back(event);
}

int GlutWindow::getWidth() const
{
	return m_width;
}

int GlutWindow::getHeight() const
{
	return m_height;
}

GlutWindow::GlutWindow(const int width, const int height) :
	m_width(width),
	m_height(height),
	m_eventQueue()
{
}

void GlutWindowNamespace::close()
{
	ms_quit = true;
}

void GlutWindowNamespace::display()
{
}

void GlutWindowNamespace::onMouse(int button, int state, int x, int y)
{
	Window::Event event;
	event.type = state == GLUT_DOWN ? Window::ET_MouseDown : Window::ET_MouseUp;
	event.mouse = { x, y };
	Window::getCurrent().addEvent(event);
}

void GlutWindowNamespace::onKeyboardDown(unsigned char key, int x, int y)
{
	Window::Event event;
	event.type = Window::ET_KeyDown;
	event.key = { key };
	Window::getCurrent().addEvent(event);
}

void GlutWindowNamespace::onKeyboardUp(unsigned char key, int x, int y)
{
	Window::Event event;
	event.type = Window::ET_KeyUp;
	event.key = { key };
	Window::getCurrent().addEvent(event);
}