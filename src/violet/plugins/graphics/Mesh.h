#ifndef MESH_H
#define MESH_H

#include "violet/core/Defines.h"
#include "violet/core/math/Vec2.h"

#include <vector>

namespace Violet
{
	class Deserializer;
	class Polygon;

	class Mesh
	{
	public:

		static void bind(const Mesh & mesh);
		static void unbind();

	public:

		explicit Mesh(std::vector<Vec2f> && vertices);
		explicit Mesh(Deserializer & deserializer);
		Mesh(Mesh && other);
		~Mesh();

	private:

		Mesh(const Mesh & other) = delete;
		Mesh & operator=(const Mesh & other) = delete;

	private:

		Mesh(const Polygon & poly);

	public:

		uint32 m_vertexBuffer;
		uint32 m_size;
	};
}

#endif
