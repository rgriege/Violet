// ============================================================================

#include "violet/graphics/component/render_component_data.h"

#include "violet/core/engine.h"
#include "violet/graphics/shader/shader.h"
#include "violet/math/poly.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/guard.h"

#include <GL/glew.h>

using namespace vlt;

// ============================================================================

render_component_data::render_component_data(deserializer & deserializer) :
	m_vertexArrayBuffer(-1),
	m_mesh(),
	m_shader(shader_program::get_cache().fetch(deserializer.get_string("shader")))
{
	const poly poly(deserializer);

	m_mesh = std::make_unique<mesh>(poly);

	glGenVertexArrays(1, &m_vertexArrayBuffer);
	glBindVertexArray(m_vertexArrayBuffer);
	const guard<mesh> meshGuard(*m_mesh);
	const GLint positionAttribute = m_shader->getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

render_component_data::render_component_data(const poly & poly, std::shared_ptr<shader_program> shader) :
	m_vertexArrayBuffer(-1),
	m_mesh(std::make_unique<mesh>(poly)),
	m_shader(std::move(shader))
{
	glGenVertexArrays(1, &m_vertexArrayBuffer);
	glBindVertexArray(m_vertexArrayBuffer);
	const guard<mesh> meshGuard(*m_mesh);
	const GLint positionAttribute = m_shader->getAttributeLocation("position");
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribute);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

render_component_data::render_component_data(const render_component_data & rhs) :
	render_component_data(rhs.m_mesh->get_poly(), rhs.m_shader)
{
}

// ----------------------------------------------------------------------------

render_component_data::render_component_data(render_component_data && other) :
	m_vertexArrayBuffer(other.m_vertexArrayBuffer),
	m_mesh(std::move(other.m_mesh)),
	m_shader(std::move(other.m_shader))
{
	other.m_vertexArrayBuffer = 0;
}

// ----------------------------------------------------------------------------

render_component_data::~render_component_data()
{
	if (m_vertexArrayBuffer != 0)
		glDeleteVertexArrays(1, &m_vertexArrayBuffer);
}

// ============================================================================

deserializer & vlt::operator>>(deserializer & deserializer, render_component_data & component)
{
	component.m_shader = shader_program::get_cache().fetch(deserializer.get_string("shader"));
	deserializer >> *component.m_mesh;
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const render_component_data & component)
{
	serializer.write_string("shader", component.m_shader->get_name().c_str());
	serializer << *component.m_mesh;
	return serializer;
}

// ============================================================================
