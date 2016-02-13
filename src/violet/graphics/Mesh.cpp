// ============================================================================

#include "violet/graphics/mesh.h"

#include "violet/math/poly.h"

#include <GL/glew.h>

using namespace vlt;

// ============================================================================

namespace MeshNamespace
{
	void setVertices(mesh & mesh, const poly & poly);
}

using namespace MeshNamespace;

// ============================================================================

void mesh::bind(const mesh & mesh)
{
	glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vertexBuffer);
}

// ----------------------------------------------------------------------------

void mesh::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ============================================================================

mesh::mesh(vector<v2> && vertices) :
	mesh(poly(std::move(vertices)))
{
}

// ----------------------------------------------------------------------------

mesh::mesh(deserializer & deserializer) :
	mesh(poly(deserializer))
{
}

// ----------------------------------------------------------------------------

mesh::mesh(const mesh & other) :
	mesh(other.get_poly())
{
}

// ----------------------------------------------------------------------------

mesh::mesh(mesh && other) :
	m_vertexBuffer(other.m_vertexBuffer),
	m_size(other.m_size)
{
	other.m_vertexBuffer = 0;
}

// ----------------------------------------------------------------------------

mesh & mesh::operator=(mesh && other)
{
	std::swap(m_vertexBuffer, other.m_vertexBuffer);
	std::swap(m_size, other.m_size);
	return *this;
}

// ----------------------------------------------------------------------------

mesh::mesh(const poly & poly) :
	m_vertexBuffer(0),
	m_size(poly.vertices.size())
{
	glGenBuffers(1, &m_vertexBuffer);
	setVertices(*this, poly);
}

// ----------------------------------------------------------------------------

mesh::~mesh()
{
	if (m_vertexBuffer != 0)
		glDeleteBuffers(1, &m_vertexBuffer);
}

// ----------------------------------------------------------------------------

poly mesh::get_poly() const
{
	vector<v2> vertices;
	vertices.resize(m_size);
	mesh::bind(*this);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_size * sizeof(v2), vertices.data());
	mesh::unbind();
	return poly(std::move(vertices));
}

// ============================================================================

deserializer & vlt::operator>>(deserializer & deserializer, mesh & mesh)
{
	poly poly(deserializer);
	setVertices(mesh, poly);
	return deserializer;
}

// ----------------------------------------------------------------------------

serializer & vlt::operator<<(serializer & serializer, const mesh & mesh)
{
	return serializer << mesh.get_poly();
}

// ============================================================================

void MeshNamespace::setVertices(mesh & mesh, const poly & poly)
{

	mesh::bind(mesh);
	glBufferData(GL_ARRAY_BUFFER, poly.vertices.size() * sizeof(v2), poly.vertices.data(), GL_STATIC_DRAW);
	mesh::unbind();
}

// ============================================================================
