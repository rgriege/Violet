#ifndef MESH_H
#define MESH_H

#include "engine/Defines.h"
#include "engine/math/Vec2.h"

#include <vector>

namespace Violet
{
	class Deserializer;
	class Polygon;
	class Serializer;

	class Mesh
	{
	public:

		static void bind(const Mesh & mesh);
		static void unbind();

	public:

		explicit Mesh(std::vector<Vec2f> && vertices);
		explicit Mesh(const Polygon & poly);
		explicit Mesh(Deserializer & deserializer);
		Mesh(Mesh && other);
		Mesh & operator=(Mesh && other);
		~Mesh();

	private:

		Mesh(const Mesh & other) = delete;
		Mesh & operator=(const Mesh & other) = delete;

	public:

		uint32 m_vertexBuffer;
		uint32 m_size;
	};

	Serializer & operator<<(Serializer & serializer, const Mesh & mesh);
}

#endif
