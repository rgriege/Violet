#include "violet/plugins/graphics/system/RenderSystem.h"

#include "violet/core/Engine.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/math/Matrix3.h"
#include "violet/core/serialization/Deserializer.h"
#include "violet/core/system/SystemFactory.h"
#include "violet/core/transform/TransformSystem.h"
#include "violet/core/utility/Guard.h"
#include "violet/plugins/graphics/font/Font.h"
#include "violet/plugins/graphics/shader/Shader.h"

#include <iostream>
#include <fstream>
#include <GL/freeglut.h>
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace Violet;

namespace RenderSystemNamespace
{
	RenderSystem * ms_renderSystem;

	Engine * ms_engine;

	Matrix3f ms_viewMatrix;

	void draw(RenderComponent & renderComponent);
	void draw(TextComponent & textComponent);
	void close();
}

using namespace RenderSystemNamespace;

const char * RenderSystem::getStaticLabel()
{
	return RenderComponent::getLabel();
}

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
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutDisplayFunc(display);
	glutCloseFunc(close);

	ms_renderSystem = new RenderSystem();
	
	return std::unique_ptr<System>(ms_renderSystem);
}

RenderSystem::RenderSystem() :
	MultiComponentSystem<RenderComponent, TextComponent>("rndr")
{
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
	for (auto & component : ms_renderSystem->getComponents<RenderComponent>())
		draw(component);
	for (auto & component : ms_renderSystem->getComponents<TextComponent>())
		draw(component);
	glFlush();
}

void RenderSystemNamespace::draw(RenderComponent & renderComponent)
{
	const TransformComponent & transform = ms_engine->fetch<TransformComponent>(renderComponent.m_entity);
	const float modelMat[9] = {
		1.f, 0.f, transform.m_position.x,
		0.f, 1.f, transform.m_position.y,
		0.f, 0.f, 1.f
	};

	const GLint colorAttrib = renderComponent.m_shader->getUniformLocation("color");
	const GLint modelAttrib = renderComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = renderComponent.m_shader->getUniformLocation("view");

	glBindVertexArray(renderComponent.m_vertexArrayBuffer);
	const Guard<ShaderProgram> shaderGuard(*renderComponent.m_shader);
	glUniform4fv(colorAttrib, 1, renderComponent.m_color.as4fv().data());
	glUniformMatrix3fv(modelAttrib, 1, true, modelMat);
	glUniformMatrix3fv(viewAttribute, 1, true, ms_viewMatrix.data());

	glDrawArrays(GL_TRIANGLE_FAN, 0, renderComponent.m_mesh.m_size);
	glBindVertexArray(0);
}

void RenderSystemNamespace::draw(TextComponent & textComponent)
{
	const TransformComponent & transform = ms_engine->fetch<TransformComponent>(textComponent.m_entity);
	float modelMat[9] = {
		1.f, 0.f, transform.m_position.x,
		0.f, 1.f, transform.m_position.y,
		0.f, 0.f, 1.f
	};

	const GLint modelAttrib = textComponent.m_shader->getUniformLocation("model");
	const GLint viewAttribute = textComponent.m_shader->getUniformLocation("view");

	const Guard<ShaderProgram> shaderGuard(*textComponent.m_shader);
	glUniformMatrix3fv(modelAttrib, 1, true, modelMat);
	glUniformMatrix3fv(viewAttribute, 1, true, ms_viewMatrix.data());

	textComponent.m_font->render(textComponent.m_text, *textComponent.m_shader);
}

void RenderSystemNamespace::close()
{
	ms_renderSystem = nullptr;
	ShaderProgram::getCache().clear();
	Font::getCache().clear();
	ms_engine->stop();
}
