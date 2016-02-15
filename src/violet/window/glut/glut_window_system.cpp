// ============================================================================

#include "violet/window/glut/glut_window_system.h"

#include "violet/core/engine.h"
#include "violet/math/v2.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/utility/memory.h"

#include <GL/freeglut.h>

using namespace vlt;

// ============================================================================

namespace GlutWindowSystemNamespace
{
	void close();
	void display();
	void onMouseButton(int button, int state, int x, int y);
	void onKeyboardDown(unsigned char key, int x, int y);
	void onKeyboardUp(unsigned char key, int x, int y);

	glut_window_system * ms_currentWindow;
}

using namespace GlutWindowSystemNamespace;

// ============================================================================

void glut_window_system::install(system_factory & factory)
{
	factory.assign(get_label_static(), &glut_window_system::init);
}

// ----------------------------------------------------------------------------

namespace
{
	struct init_task_data
	{
		int x;
		int y;
		int width;
		int height;
		std::string title;
		u32 thread;
	};
}

static void init_task(void * mem)
{
	auto data = make_unique<init_task_data>(mem);

	int argc = 0;
	char arr[1] = { '\0' };
	char * argv = arr;

	glutInit(&argc, &argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(data->width, data->height);
	glutInitWindowPosition(data->x, data->y);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	const int id = glutCreateWindow(data->title.c_str());
	if (id == 0)
	{
		engine::instance().stop();
		return;
	}

	glutCloseFunc(close);
	glutDisplayFunc(display);
	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMove);
	glutKeyboardFunc(onKeyboardDown);
	glutKeyboardUpFunc(onKeyboardUp);

	engine::instance().add_system(std::unique_ptr<vlt::system>(new glut_window_system(id, data->width, data->height)));
}

void glut_window_system::init(deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());

	auto data = new init_task_data;
	data->x = settingsSegment->get_s32("x");
	data->y = settingsSegment->get_s32("y");
	data->width = settingsSegment->get_s32("width");
	data->height = settingsSegment->get_s32("height");
	data->title = settingsSegment->get_string("title");
	data->thread = settingsSegment->get_u32("thread");

	add_task(init_task, data, data->thread, task_type::write);
}

// ============================================================================

glut_window_system::glut_window_system(const int id, const int width, const int height) :
	window_system(),
	m_id(id),
	m_width(width),
	m_height(height),
	m_eventQueue()
{
}

// ----------------------------------------------------------------------------

void glut_window_system::update(const r32 /*dt*/)
{
	m_eventQueue.clear();
	glutSetWindow(m_id);
	ms_currentWindow = this;
	glutMainLoopEvent();
}

// ----------------------------------------------------------------------------

void glut_window_system::render()
{
	glutPostRedisplay();
}

// ----------------------------------------------------------------------------

bool glut_window_system::get_event(const event_type type, event * const event)
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

// ----------------------------------------------------------------------------

void glut_window_system::add_event(event event)
{
	m_eventQueue.push_back(event);
}

// ----------------------------------------------------------------------------

int glut_window_system::get_width() const
{
	return m_width;
}

// ----------------------------------------------------------------------------

int glut_window_system::get_height() const
{
	return m_height;
}

// ============================================================================

void GlutWindowSystemNamespace::close()
{
	window_system::event e;
	e.type = window_system::ET_Quit;
	ms_currentWindow->add_event(e);
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::display()
{
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onMouseButton(int button, int state, int x, int y)
{
	window_system::event event;
	event.type = state == GLUT_DOWN ? window_system::ET_MouseDown : window_system::ET_MouseUp;
	event.mouse = { x, y, static_cast<mouse_button>(button) };
	ms_currentWindow->add_event(event);
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onMouseMove(int x, int y)
{
	window_system::event event;
	event.type = window_system::ET_MouseMove;
	event.motion = { x, y, ms_currentWindow->m_mousePos.x - x, ms_currentWindow->m_mousePos.y - y };
	ms_currentWindow->add_event(event);
	ms_currentWindow->m_mousePos = { x, y };
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onKeyboardDown(unsigned char key, int x, int y)
{
	window_system::event event;
	event.type = window_system::ET_KeyDown;
	event.key = { key };
	ms_currentWindow->add_event(event);
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onKeyboardUp(unsigned char key, int x, int y)
{
	window_system::event event;
	event.type = window_system::ET_KeyUp;
	event.key = { key };
	ms_currentWindow->add_event(event);
}

// ============================================================================
