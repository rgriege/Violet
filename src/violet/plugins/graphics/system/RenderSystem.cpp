#include "violet/plugins/graphics/system/RenderSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/math/Matrix3.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/plugins/graphics/shader/Shader.h"

#include <iostream>
#include <fstream>
#include <GL/freeglut.h>
#include <GL/glew.h>

using namespace Violet;

namespace RenderSystemNamespace
{
	RenderSystem * ms_renderSystem;

	Engine * ms_engine;

	Matrix3f ms_viewMatrix;

	void draw(RenderComponent & renderComponent);
	void close(System *);
}

using namespace RenderSystemNamespace;

void RenderSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &RenderSystem::init);
}

std::unique_ptr<System> RenderSystem::init(Deserializer & deserializer)
{
	auto settingsSegment = deserializer.enterSegment(getStaticLabel());

	if (ms_renderSystem != nullptr)
		return nullptr;

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
	if (glutCreateWindow(title) == 0)
		return nullptr;
	
	std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << "glewInit error: " << glewGetErrorString(glewError);
		return nullptr;
	}

	ms_viewMatrix = Matrix3f::Identity;
	ms_viewMatrix[0][0] = 2.f / static_cast<float>(width);
	ms_viewMatrix[1][1] = 2.f / static_cast<float>(height);
	
	glClearColor(color.r, color.g, color.b, color.a);

	glutDisplayFunc(display);

	ms_renderSystem = new RenderSystem();
	
	return std::unique_ptr<System>(ms_renderSystem);
}

void RenderSystem::update(float const /*dt*/, Engine & engine)
{
	ms_engine = &engine;
	glutPostRedisplay();
	glutMainLoopEvent();
}

void RenderSystem::display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (auto & component : *ms_renderSystem->m_components)
		draw(component);
	glFlush();
}

void RenderSystemNamespace::draw(RenderComponent & renderComponent)
{
	const TransformComponent & transform = ms_engine->fetch<TransformSystem>(renderComponent.m_entity);
	const float modelMat[9] = {
		1.f, 0.f, transform.m_position.x,
		0.f, 1.f, transform.m_position.y,
		0.f, 0.f, 1.f
	};

	const GLint colorAttrib = renderComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = renderComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = renderComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(renderComponent.m_vertexArrayBuffer);
	auto const shaderGuard = renderComponent.m_shader->use();
	glUniform4fv(colorAttrib, 1, renderComponent.m_color.as4fv().data());
	glUniformMatrix3fv(modelAttrib, 1, true, modelMat);
	glUniformMatrix3fv(viewAttribute, 1, true, ms_viewMatrix.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, renderComponent.m_mesh.m_vertices.size());
}

void RenderSystemNamespace::close(System *)
{
	ms_renderSystem = nullptr;
}