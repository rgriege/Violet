#include "violet/plugins/graphics/Mesh.h"

#include <GL\glew.h>

using namespace Violet;

Mesh::Mesh(std::vector<Vec2f> && vertices) :
	Polygon(std::move(vertices)),
	m_vertexBuffer()
{
}

Mesh::Mesh(Deserializer & deserializer) :
	Polygon(deserializer),
	m_vertexBuffer()
{
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vec2f), m_vertices.data(), GL_STATIC_DRAW);
}