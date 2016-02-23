// ============================================================================

#include <GL/freeglut.h>

#include "violet/core/engine.h"
#include "violet/math/v2.h"
#include "violet/serialization/deserializer.h"
#include "violet/system/system_factory.h"
#include "violet/utility/memory.h"
#include "violet/window/glut/glut_window_system.h"

using namespace vlt;

// ============================================================================

namespace GlutWindowSystemNamespace
{
	void close();
	void display();
	void onMouseButton(int button, int state, int x, int y);
	void onKeyboardDown(unsigned char Key, int x, int y);
	void onKeyboardUp(unsigned char Key, int x, int y);

	Glut_Window_System * ms_currentWindow;
}

using namespace GlutWindowSystemNamespace;

// ============================================================================

void Glut_Window_System::install(System_Factory & factory)
{
	factory.assign(get_label_static(), &Glut_Window_System::init);
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
		Engine::instance().stop();
		return;
	}

	glutCloseFunc(close);
	glutDisplayFunc(display);
	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMove);
	glutKeyboardFunc(onKeyboardDown);
	glutKeyboardUpFunc(onKeyboardUp);

	Engine::instance().add_system(std::unique_ptr<vlt::System>(new Glut_Window_System(id, data->width, data->height)));
}

void Glut_Window_System::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enter_segment(get_label_static());

	auto data = new init_task_data;
	data->x = settingsSegment->get_s32("x");
	data->y = settingsSegment->get_s32("y");
	data->width = settingsSegment->get_s32("width");
	data->height = settingsSegment->get_s32("height");
	data->title = settingsSegment->get_string("title");
	data->thread = settingsSegment->get_u32("Thread");

	add_task(init_task, data, data->thread, task_type::write);
}

// ============================================================================

Glut_Window_System::Glut_Window_System(const int id, const int width, const int height) :
	Window_System(),
	m_id(id),
	m_width(width),
	m_height(height),
	m_eventQueue()
{
}

// ----------------------------------------------------------------------------

void Glut_Window_System::update(const r32 /*dt*/)
{
	m_eventQueue.clear();
	glutSetWindow(m_id);
	ms_currentWindow = this;
	glutMainLoopEvent();
}

// ----------------------------------------------------------------------------

void Glut_Window_System::render()
{
	glutPostRedisplay();
}

// ----------------------------------------------------------------------------

bool Glut_Window_System::get_event(const Event_Type type, Event * const event)
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

void Glut_Window_System::add_event(Event event)
{
	m_eventQueue.push_back(event);
}

// ----------------------------------------------------------------------------

int Glut_Window_System::get_width() const
{
	return m_width;
}

// ----------------------------------------------------------------------------

int Glut_Window_System::get_height() const
{
	return m_height;
}

// ============================================================================

void GlutWindowSystemNamespace::close()
{
	Window_System::Event e;
	e.type = Window_System::ET_Quit;
	ms_currentWindow->add_event(e);
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::display()
{
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onMouseButton(int button, int state, int x, int y)
{
	Window_System::Event event;
	event.type = state == GLUT_DOWN ? Window_System::ET_MouseDown : Window_System::ET_MouseUp;
	event.mouse = { x, y, static_cast<Mouse_Button>(button) };
	ms_currentWindow->add_event(event);
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onMouseMove(int x, int y)
{
	Window_System::Event event;
	event.type = Window_System::ET_MouseMove;
	event.motion = { x, y, ms_currentWindow->m_mousePos.x - x, ms_currentWindow->m_mousePos.y - y };
	ms_currentWindow->add_event(event);
	ms_currentWindow->m_mousePos = { x, y };
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onKeyboardDown(unsigned char key, int x, int y)
{
	Window_System::Event event;
	event.type = Window_System::ET_KeyDown;
	event.key = { key };
	ms_currentWindow->add_event(event);
}

// ----------------------------------------------------------------------------

void GlutWindowSystemNamespace::onKeyboardUp(unsigned char key, int x, int y)
{
	Window_System::Event event;
	event.type = Window_System::ET_KeyUp;
	event.key = { key };
	ms_currentWindow->add_event(event);
}

// ============================================================================
