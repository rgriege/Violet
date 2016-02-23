// ============================================================================

#include <GL/glew.h>

#include "violet/graphics/mesh.h"
#include "violet/math/poly.h"

using namespace vlt;

// ============================================================================

void Mesh::bind(const Mesh & mesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
}

// ----------------------------------------------------------------------------

void Mesh::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ============================================================================

Mesh::Mesh(Vector<v2> && vertices) :
	Mesh(Poly(std::move(vertices)))
{
}

// ----------------------------------------------------------------------------

Mesh::Mesh(Deserializer & deserializer) :
	Mesh(Poly(deserializer))
{
}

// ----------------------------------------------------------------------------

Mesh::Mesh(const Mesh & other) :
	Mesh(other.get_poly())
{
}

// ----------------------------------------------------------------------------

Mesh::Mesh(Mesh && other) :
	vertex_buffer(other.vertex_buffer),
	size(other.size)
{
	other.vertex_buffer = 0;
}

// ----------------------------------------------------------------------------

Mesh & Mesh::operator=(Mesh && other)
{
	std::swap(vertex_buffer, other.vertex_buffer);
	std::swap(size, other.size);
	return *this;
}

// ----------------------------------------------------------------------------

Mesh::Mesh(const Poly & poly) :
	vertex_buffer(0),
	size(poly.vertices.size())
{
	glGenBuffers(1, &vertex_buffer);
	set_mesh_vertices(*this, poly.vertices);
}

// ----------------------------------------------------------------------------

Mesh::~Mesh()
{
	if (vertex_buffer != 0)
		glDeleteBuffers(1, &vertex_buffer);
}

// ----------------------------------------------------------------------------

Poly Mesh::get_poly() const
{
	Vector<v2> vertices;
	vertices.resize(size);
	Mesh::bind(*this);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(v2), vertices.data());
	Mesh::unbind();
	return Poly(std::move(vertices));
}

// ============================================================================

void vlt::set_mesh_vertices(Mesh & mesh, const Vector<v2> & vertices)
{
	Mesh::bind(mesh);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(v2), vertices.data(), GL_STATIC_DRAW);
	Mesh::unbind();
	mesh.size = vertices.size();
}

// ----------------------------------------------------------------------------

Deserializer & vlt::operator>>(Deserializer & deserializer, Mesh & mesh)
{
	Poly poly(deserializer);
	set_mesh_vertices(mesh, poly.vertices);
	return deserializer;
}

// ----------------------------------------------------------------------------

Serializer & vlt::operator<<(Serializer & serializer, const Mesh & mesh)
{
	return serializer << mesh.get_poly();
}

// ============================================================================
