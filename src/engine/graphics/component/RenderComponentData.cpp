// ============================================================================

#include "engine/graphics/component/RenderComponentData.h"

#include "engine/graphics/shader/Shader.h"
#include "engine/math/Polygon.h"
#include "engine/serialization/Deserializer.h"
#include "engine/serialization/Serializer.h"
#include "engine/utility/Guard.h"

#include <GL/glew.h>

using namespace Violet;

// ============================================================================

namespace RenderComponentDataNamespace
{
	GLuint initVertexArrayBuffer();
};

using namespace RenderComponentDataNamespace;

// ============================================================================

RenderComponentData::RenderComponentData(Deserializer & deserializer) :
	RenderComponentData(Polygon(deserializer), ShaderProgram::getCache().fetch(deserializer.getString("shader")))
{
}

// ----------------------------------------------------------------------------

RenderComponentData::RenderComponentData(const Polygon & poly, std::shared_ptr<ShaderProgram> shader) :
	m_vertexArrayBuffer(initVertexArrayBuffer()),
	m_mesh(poly),
	m_shader(std::move(shader))
{
	const Guard<Mesh> meshGuard(m_mesh);
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
	deserializer >> component.m_mesh;
	component.m_shader = ShaderProgram::getCache().fetch(deserializer.getString("shader"));
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & Violet::operator<<(Serializer & serializer, const RenderComponentData & component)
{
	serializer << component.m_mesh;
	serializer.writeString("shader", component.m_shader->getName().c_str());
	return serializer;
}

// ============================================================================

GLuint RenderComponentDataNamespace::initVertexArrayBuffer()
{
	GLuint vertexArrayBuffer;
	glGenVertexArrays(1, &vertexArrayBuffer);
	glBindVertexArray(vertexArrayBuffer);
	return vertexArrayBuffer;
}

// ============================================================================
