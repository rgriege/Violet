#include "violet/plugins/graphics/component/RenderComponent.h"

#include "violet/core/serialization/Deserializer.h"
#include "violet/core/utility/Guard.h"
#include "violet/plugins/graphics/shader/Shader.h"

#include <GL\glew.h>

using namespace Violet;

namespace RenderComponentNamespace
{
	GLuint initVertexArrayBuffer();
};

using namespace RenderComponentNamespace;

const char * RenderComponent::getLabel()
{
	return "rndr";
}

RenderComponent::RenderComponent(const Entity & entity, Deserializer & deserializer) :
	Component(entity),
	m_vertexArrayBuffer(initVertexArrayBuffer()),
	m_mesh(deserializer),
	m_color(deserializer),
	m_shader(ShaderProgram::getCache().fetch(deserializer.getString("shader")))
{
	const Guard<Mesh> meshGuard(m_mesh);
	const GLint positionAttribute = m_shader->getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	glBindVertexArray(0);
}

RenderComponent::RenderComponent(RenderComponent && other) :
	Component(std::move(other)),
	m_mesh(std::move(other.m_mesh)),
	m_color(std::move(other.m_color)),
	m_vertexArrayBuffer(other.m_vertexArrayBuffer),
	m_shader()
{
	m_shader.swap(other.m_shader);
	other.m_vertexArrayBuffer = 0;
}

RenderComponent::~RenderComponent()
{
	if (m_vertexArrayBuffer != 0)
		glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

GLuint RenderComponentNamespace::initVertexArrayBuffer()
{
	GLuint vertexArrayBuffer;
	glGenVertexArrays(1, &vertexArrayBuffer);
	glBindVertexArray(vertexArrayBuffer);
	return vertexArrayBuffer;
}