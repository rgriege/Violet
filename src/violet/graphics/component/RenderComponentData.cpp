// ============================================================================

#include "violet/graphics/component/RenderComponentData.h"

#include "violet/Engine.h"
#include "violet/graphics/shader/Shader.h"
#include "violet/math/Polygon.h"
#include "violet/serialization/Deserializer.h"
#include "violet/serialization/Serializer.h"
#include "violet/utility/Guard.h"

#include <GL/glew.h>

using namespace Violet;

// ============================================================================

RenderComponentData::RenderComponentData(Deserializer & deserializer) :
	m_vertexArrayBuffer(-1),
	m_mesh(),
	m_shader(ShaderProgram::getCache().fetch(deserializer.getString("shader")))
{
	const Polygon poly(deserializer);

	m_mesh = std::make_unique<Mesh>(poly);

	glGenVertexArrays(1, &m_vertexArrayBuffer);
	glBindVertexArray(m_vertexArrayBuffer);
	const Guard<Mesh> meshGuard(*m_mesh);
	const GLint positionAttribute = m_shader->getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

RenderComponentData::RenderComponentData(const Polygon & poly, std::shared_ptr<ShaderProgram> shader) :
	m_vertexArrayBuffer(-1),
	m_mesh(std::make_unique<Mesh>(poly)),
	m_shader(std::move(shader))
{
	glGenVertexArrays(1, &m_vertexArrayBuffer);
	glBindVertexArray(m_vertexArrayBuffer);
	const Guard<Mesh> meshGuard(*m_mesh);
	const GLint positionAttribute = m_shader->getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

RenderComponentData::RenderComponentData(RenderComponentData && other) :
	m_vertexArrayBuffer(other.m_vertexArrayBuffer),
	m_mesh(std::move(other.m_mesh)),
	m_shader(std::move(other.m_shader))
{
	other.m_vertexArrayBuffer = 0;
}

// ----------------------------------------------------------------------------

RenderComponentData::~RenderComponentData()
{
	if (m_vertexArrayBuffer != 0)
		glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

// ============================================================================

Deserializer & Violet::operator>>(Deserializer & deserializer, RenderComponentData & component)
{
	component.m_shader = ShaderProgram::getCache().fetch(deserializer.getString("shader"));
	deserializer >> *component.m_mesh;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const RenderComponentData & component)
{
	serializer.writeString("shader", component.m_shader->getName().c_str());
	serializer << *component.m_mesh;
	return serializer;
}

// ============================================================================
