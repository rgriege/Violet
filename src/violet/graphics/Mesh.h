#ifndef VIOLET_Mesh_H
#define VIOLET_Mesh_H

#include "violet/Defines.h"
#include "violet/math/Vec2.h"

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
		explicit Mesh(const Mesh & other);
		explicit Mesh(Mesh && other);
		Mesh & operator=(Mesh && other);
		~Mesh();

		Polygon getPolygon() const;

	private:

		Mesh & operator=(const Mesh & other) = delete;

	public:

		uint32 m_vertexBuffer;
		uint32 m_size;
	};

	Deserializer & operator>>(Deserializer & deserializer, Mesh & mesh);
	Serializer & operator<<(Serializer & serializer, const Mesh & mesh);
}

#endif
