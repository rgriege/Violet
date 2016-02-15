// ============================================================================

#include <GL/glew.h>

#include "violet/component/component_deserializer.h"
#include "violet/component/scene.h"
#include "violet/graphics/component/texture_component.h"
#include "violet/graphics/shader/shader.h"
#include "violet/math/aabb.h"
#include "violet/math/poly.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/guard.h"

using namespace vlt;

// ============================================================================

const component_metadata * texture_component::metadata;

// ============================================================================

vector<v2> create_text_coords_from_mesh(const mesh & mesh);

// ============================================================================

texture_component::texture_component(const handle entity_id, component_deserializer & deserializer) :
	render_component_data(deserializer),
	m_texture(texture::get_cache().fetch(deserializer.get_string("texture"))),
	m_texCoords(std::make_unique<mesh>(create_text_coords_from_mesh(*m_mesh)))
{
	glBindVertexArray(m_vertexArrayBuffer);
	const guard<mesh> texCoordGuard(*m_texCoords);
	const GLuint texCoordAttrib = m_shader->getAttributeLocation("texCoord");
	glEnableVertexAttribArray(texCoordAttrib);
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

texture_component::texture_component(const handle entity_id, const poly & p, std::shared_ptr<shader_program> shader, std::shared_ptr<texture> texture, const poly & texCoords) :
	render_component_data(p, shader),
	m_texture(std::move(texture)),
	m_texCoords(std::make_unique<mesh>(texCoords))
{
}

// ----------------------------------------------------------------------------

texture_component::texture_component(texture_component && other) :
	render_component_data(std::move(other)),
	m_texture(),
	m_texCoords(std::move(other.m_texCoords))
{
	m_texture.swap(other.m_texture);
}

// ============================================================================

void vlt::install_texture_component()
{
	texture_component::metadata = init_component_metadata(tag('t', 'e', 'x', 'u'), 0, sizeof(texture_component));
	scene::install_component<texture_component>();
}

// ----------------------------------------------------------------------------

component_deserializer & vlt::operator>>(component_deserializer & deserializer, texture_component & component)
{
	operator>>(deserializer, static_cast<render_component_data &>(component));
	component.m_texture = texture::get_cache().fetch(deserializer.get_string("texture"));
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const texture_component & component)
{
	operator<<(serializer, static_cast<const render_component_data &>(component));
	serializer.write_string("texture", component.m_texture->get_name().c_str());
	return serializer;
}

// ============================================================================

vector<v2> create_text_coords_from_mesh(const mesh & mesh)
{
	aabb extent;
	const auto poly = mesh.get_poly();
	for (const auto & vertex : poly.vertices)
		extent.extend(vertex);
	const v2 & minimum = extent.get_min();
	const v2 & dimension = extent.get_max() - minimum;

	vector<v2> result;
	result.reserve(poly.vertices.size());
	for (const v2 & vertex : poly.vertices)
		result.emplace_back(v2((vertex.x - minimum.x) / dimension.x, (vertex.y - minimum.y) / dimension.y));
	return result;
}

	// ============================================================================
