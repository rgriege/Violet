#include "violet/plugins/glut/GlutWindowSystem.h"

#include "violet/core/math/Vec2.h"
#include "violet/core/serialization/Deserializer.h"

#include <GL/freeglut.h>

using namespace Violet;

namespace GlutWindowSystemNamespace
{
	void close();
	void display();
	void onMouseButton(int button, int state, int x, int y);
	void onKeyboardDown(unsigned char key, int x, int y);
	void onKeyboardUp(unsigned char key, int x, int y);

	GlutWindowSystem * ms_currentWindow;
}

using namespace GlutWindowSystemNamespace;

void GlutWindowSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &GlutWindowSystem::init);
}

std::unique_ptr<System> GlutWindowSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());

	int argc = 0;
	char * argv = "";
	int const x = settingsSegment->getInt("x");
	int const y = settingsSegment->getInt("y");
	int const width = settingsSegment->getInt("width");
	int const height = settingsSegment->getInt("height");
	const char * title = settingsSegment->getString("title");

	glutInit(&argc, &argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(x, y);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	const int id = glutCreateWindow(title);
	if (id == 0)
		return nullptr;

	glutCloseFunc(close);
	glutDisplayFunc(display);
	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMove);
	glutKeyboardFunc(onKeyboardDown);
	glutKeyboardUpFunc(onKeyboardUp);

	return std::unique_ptr<System>(new GlutWindowSystem(id, width, height));
}

void GlutWindowSystem::update(float /*dt*/, Engine & engine)
{
	m_eventQueue.clear();
	glutSetWindow(m_id);
	ms_currentWindow = this;
	glutMainLoopEvent();
}

void GlutWindowSystem::render()
{
	glutPostRedisplay();
}

bool GlutWindowSystem::getEvent(const EventType type, Event * const event)
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

void GlutWindowSystem::addEvent(Event event)
{
	m_eventQueue.push_back(event);
}

int GlutWindowSystem::getWidth() const
{
	return m_width;
}

int GlutWindowSystem::getHeight() const
{
	return m_height;
}

GlutWindowSystem::GlutWindowSystem(const int id, const int width, const int height) :
	WindowSystem(),
	m_id(id),
	m_width(width),
	m_height(height),
	m_eventQueue()
{
}

void GlutWindowSystemNamespace::close()
{
	WindowSystem::Event e;
	e.type = WindowSystem::ET_Quit;
	ms_currentWindow->addEvent(e);
}

void GlutWindowSystemNamespace::display()
{
}

void GlutWindowSystemNamespace::onMouseButton(int button, int state, int x, int y)
{
	WindowSystem::Event event;
	event.type = state == GLUT_DOWN ? WindowSystem::ET_MouseDown : WindowSystem::ET_MouseUp;
	event.mouse = { x, y };
	ms_currentWindow->addEvent(event);
}

void GlutWindowSystemNamespace::onMouseMove(int x, int y)
{
	WindowSystem::Event event;
	event.type = WindowSystem::ET_MouseMove;
	event.motion = { x, y, ms_currentWindow->m_mousePos.x - x, ms_currentWindow->m_mousePos.y - y };
	ms_currentWindow->addEvent(event);
	ms_currentWindow->m_mousePos = { x, y };
}

void GlutWindowSystemNamespace::onKeyboardDown(unsigned char key, int x, int y)
{
	WindowSystem::Event event;
	event.type = WindowSystem::ET_KeyDown;
	event.key = { key };
	ms_currentWindow->addEvent(event);
}

void GlutWindowSystemNamespace::onKeyboardUp(unsigned char key, int x, int y)
{
	WindowSystem::Event event;
	event.type = WindowSystem::ET_KeyUp;
	event.key = { key };
	ms_currentWindow->addEvent(event);
}