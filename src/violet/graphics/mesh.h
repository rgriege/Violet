#ifndef VIOLET_MESH_H
#define VIOLET_MESH_H

#include "violet/core/defines.h"
#include "violet/math/v2.h"
#include "violet/structures/vector.h"

namespace vlt
{
	struct Deserializer;
	struct Poly;
	struct Serializer;

	struct VIOLET_API Mesh final
	{
		u32 vertex_buffer;
		u32 size;

		static void bind(const Mesh & mesh);
		static void unbind();

		explicit Mesh(Vector<v2> && vertices);
		explicit Mesh(const Poly & poly);
		explicit Mesh(Deserializer & deserializer);
		explicit Mesh(const Mesh & other);
		explicit Mesh(Mesh && other);
		Mesh & operator=(Mesh && other);
		~Mesh();

		Poly get_poly() const;

	private:

		Mesh & operator=(const Mesh & other) = delete;
	};

	VIOLET_API void set_mesh_vertices(Mesh & mesh, const Vector<v2> & vertices);

	Deserializer & operator>>(Deserializer & deserializer, Mesh & mesh);
	Serializer & operator<<(Serializer & serializer, const Mesh & mesh);
}

#endif
