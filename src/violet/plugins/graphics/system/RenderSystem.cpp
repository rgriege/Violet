#include "violet/plugins/graphics/system/RenderSystem.h"

#include "violet/core/math/Circle.h"
#include "violet/core/math/Constants.h"
#include "violet/core/math/Polygon.h"

#include <GL/glut.h>

using namespace Violet;

namespace RenderSystemNamespace
{
	RenderSystem * ms_renderSystem;

	void draw(const Violet::Polygon & polygon, const Color & color);
}
using namespace RenderSystemNamespace;

RenderSystem * RenderSystem::init(Settings & settings)
{
	if (ms_renderSystem != nullptr)
		return false;

	glutInit(&settings.argc, settings.argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(settings.width, settings.height);
	glutInitWindowPosition(settings.x, settings.y);
	if (glutCreateWindow(settings.title) == 0)
		return false;

	glClearColor(Color::kTan.r, Color::kTan.g, Color::kTan.b, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, settings.width, 0, settings.height);
	glMatrixMode(GL_MODELVIEW);

	ms_renderSystem = new RenderSystem();
	glutDisplayFunc(display);
	return ms_renderSystem;
}

void RenderSystem::update(float const /*dt*/)
{
	glutPostRedisplay();
}

void RenderSystem::display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (auto const & component : ms_renderSystem->m_components)
		draw(component.m_mesh, component.m_color);
	glFlush();
}

void RenderSystemNamespace::draw(const Violet::Polygon & polygon, const Color & color)
{
	glColor3f(color.r, color.g, color.b);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	const size_t len = polygon.vertices.size();
	for (size_t i = 0; i < len; i++)
		glVertex2f(polygon.center.x + polygon.vertices[i].x, polygon.center.y + polygon.vertices[i].y);
	glEnd();
}