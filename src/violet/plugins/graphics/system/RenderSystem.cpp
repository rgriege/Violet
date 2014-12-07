#include "violet/plugins/graphics/system/RenderSystem.h"

#include "violet/core/AlterContext.h"
#include "violet/core/component/ComponentFactory.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/plugins/graphics/Mesh.h"

#include <GL/freeglut.h>

using namespace Violet;

namespace RenderSystemNamespace
{
	RenderSystem * ms_renderSystem;

	AlterContext * ms_alterContext;

	void draw(RenderComponent & renderComponent);
	void close(System *);
}

using namespace RenderSystemNamespace;

void RenderSystem::install(SystemFactory & factory)
{
	factory.assign(RenderSystem::getStaticLabel(), &RenderSystem::init);
}

std::unique_ptr<System> RenderSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());

	bool succeeded = true;
	if (ms_renderSystem == nullptr)
	{
		int argc = 0;
		char * argv = "";
		int const x = settingsSegment->getInt("x");
		int const y = settingsSegment->getInt("y");
		int const width = settingsSegment->getInt("width");
		int const height = settingsSegment->getInt("height");
		const char * title = settingsSegment->getString("title");
		Color const color(*settingsSegment);

		glutInit(&argc, &argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(width, height);
		glutInitWindowPosition(x, y);
		if (glutCreateWindow(title) != 0)
		{
			glClearColor(color.r, color.g, color.b, color.a);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, width, 0, height);
			glMatrixMode(GL_MODELVIEW);

			glutDisplayFunc(display);

			ms_renderSystem = new RenderSystem();
			ComponentFactory::getInstance().assign(RenderComponent::getLabel(), std::bind(&RenderSystem::create, ms_renderSystem, std::placeholders::_1, std::placeholders::_2));
		}
		else
			succeeded = false;
	}

	return succeeded ? std::unique_ptr<System>(ms_renderSystem) : nullptr;
}

void RenderSystem::update(float const /*dt*/, AlterContext & context)
{
	ms_alterContext = &context;
	glutPostRedisplay();
	glutMainLoopEvent();
}

void RenderSystem::display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (auto & component : ms_renderSystem->m_components)
		draw(component);
	glFlush();
}

void RenderSystemNamespace::draw(RenderComponent & renderComponent)
{
	glPushMatrix();
	const TransformComponent & transform = ms_alterContext->fetch<TransformSystem>(renderComponent.m_entity);
	glTranslatef(transform.m_position.x, transform.m_position.y, 0.f);
	glColor4f(renderComponent.m_color.r, renderComponent.m_color.g, renderComponent.m_color.b, renderComponent.m_color.a);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	const size_t len = renderComponent.m_mesh.m_vertices.size();
	for (size_t i = 0; i < len; i++)
		glVertex2f(renderComponent.m_mesh.m_vertices[i].x, renderComponent.m_mesh.m_vertices[i].y);
	glEnd();
	glPopMatrix();
}

void RenderSystemNamespace::close(System *)
{
	ms_renderSystem = nullptr;
}