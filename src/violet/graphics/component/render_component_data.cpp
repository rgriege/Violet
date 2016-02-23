// ============================================================================

#include <GL/glew.h>

#include "violet/core/engine.h"
#include "violet/graphics/component/render_component_data.h"
#include "violet/graphics/shader/shader.h"
#include "violet/math/poly.h"
#include "violet/serialization/deserializer.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/guard.h"

using namespace vlt;

// ============================================================================

Render_Component_Data::Render_Component_Data(Deserializer & deserializer) :
	vertex_array_buffer(-1),
	mesh(),
	shader(Shader_Program::get_cache().fetch(deserializer.get_string("shader")))
{
	const Poly poly(deserializer);

	mesh = std::make_unique<Mesh>(poly);

	glGenVertexArrays(1, &vertex_array_buffer);
	glBindVertexArray(vertex_array_buffer);
	const Guard<Mesh> mesh_guard(*mesh);
	const GLint pos_attrib = shader->get_attrib_loc("position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pos_attrib);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

Render_Component_Data::Render_Component_Data(const Poly & poly, std::shared_ptr<Shader_Program> _shader) :
	vertex_array_buffer(-1),
	mesh(std::make_unique<Mesh>(poly)),
	shader(std::move(_shader))
{
	glGenVertexArrays(1, &vertex_array_buffer);
	glBindVertexArray(vertex_array_buffer);
	const Guard<Mesh> mesh_guard(*mesh);
	const GLint pos_attrib = shader->get_attrib_loc("position");
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(pos_attrib);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

Render_Component_Data::Render_Component_Data(const Render_Component_Data & rhs) :
	Render_Component_Data(rhs.mesh->get_poly(), rhs.shader)
{
}

// ----------------------------------------------------------------------------

Render_Component_Data::Render_Component_Data(Render_Component_Data && other) :
	vertex_array_buffer(other.vertex_array_buffer),
	mesh(std::move(other.mesh)),
	shader(std::move(other.shader))
{
	other.vertex_array_buffer = 0;
}

// ----------------------------------------------------------------------------

Render_Component_Data::~Render_Component_Data()
{
	if (vertex_array_buffer != 0)
		glDeleteVertexArrays(1, &vertex_array_buffer);
}

// ============================================================================

Deserializer & vlt::operator>>(Deserializer & deserializer, Render_Component_Data & component)
{
	component.shader = Shader_Program::get_cache().fetch(deserializer.get_string("shader"));
	deserializer >> *component.mesh;
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Render_Component_Data & component)
{
	serializer.write_string("shader", component.shader->get_name().c_str());
	serializer << *component.mesh;
	return serializer;
}

// ============================================================================
