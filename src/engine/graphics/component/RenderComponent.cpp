// ============================================================================

#include "engine/graphics/component/RenderComponent.h"

#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"
#include "engine/utility/Guard.h"
#include "engine/graphics/shader/Shader.h"

#include <GL/glew.h>

using namespace Violet;

// ============================================================================

namespace RenderComponentNamespace
{
	GLuint initVertexArrayBuffer();
};

using namespace RenderComponentNamespace;

// ============================================================================

Tag RenderComponent::getTag()
{
	return Tag('r', 'n', 'd', 'r');
}

// ============================================================================

RenderComponent::RenderComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<RenderComponent>(owner),
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

// ----------------------------------------------------------------------------

RenderComponent::RenderComponent(const Entity & owner, const Polygon & poly, Color color, std::shared_ptr<ShaderProgram> shader) :
	ComponentBase<RenderComponent>(owner),
	m_vertexArrayBuffer(initVertexArrayBuffer()),
	m_mesh(poly),
	m_color(color),
	m_shader(std::move(shader))
{
	const Guard<Mesh> meshGuard(m_mesh);
	const GLint positionAttribute = m_shader->getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

RenderComponent::RenderComponent(RenderComponent && other) :
	ComponentBase<RenderComponent>(std::move(other)),
	m_mesh(std::move(other.m_mesh)),
	m_color(std::move(other.m_color)),
	m_vertexArrayBuffer(other.m_vertexArrayBuffer),
	m_shader()
{
	m_shader.swap(other.m_shader);
	other.m_vertexArrayBuffer = 0;
}

// ----------------------------------------------------------------------------

//RenderComponent & RenderComponent::operator=(RenderComponent && other)
//{
//	Component::operator=(std::move(other));
//	std::swap(m_mesh, other.m_mesh);
//	std::swap(m_color, other.m_color);
//	std::swap(m_vertexArrayBuffer, other.m_vertexArrayBuffer);
//	std::swap(m_shader, other.m_shader);
//	return *this;
//}

// ----------------------------------------------------------------------------

RenderComponent::~RenderComponent()
{
	if (m_vertexArrayBuffer != 0)
		glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, RenderComponent & component)
{
	deserializer >> component.m_mesh >> component.m_color;
	component.m_shader = ShaderProgram::getCache().fetch(deserializer.getString("shader"));
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const RenderComponent & component)
{
	serializer << component.m_mesh << component.m_color;
	serializer.writeString("shader", component.m_shader->getName().c_str());
	return serializer;
}

// ============================================================================

GLuint RenderComponentNamespace::initVertexArrayBuffer()
{
	GLuint vertexArrayBuffer;
	glGenVertexArrays(1, &vertexArrayBuffer);
	glBindVertexArray(vertexArrayBuffer);
	return vertexArrayBuffer;
}

// ============================================================================
