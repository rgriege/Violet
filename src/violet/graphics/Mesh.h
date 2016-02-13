#ifndef VIOLET_MESH_H
#define VIOLET_MESH_H

#include "violet/core/defines.h"
#include "violet/math/v2.h"
#include "violet/structures/vector.h"

namespace vlt
{
	struct deserializer;
	struct poly;
	struct serializer;

	struct VIOLET_API mesh final
	{
		u32 m_vertexBuffer;
		u32 m_size;

		static void bind(const mesh & mesh);
		static void unbind();

		explicit mesh(vector<v2> && vertices);
		explicit mesh(const poly & poly);
		explicit mesh(deserializer & deserializer);
		explicit mesh(const mesh & other);
		explicit mesh(mesh && other);
		mesh & operator=(mesh && other);
		~mesh();

		poly get_poly() const;

	private:

		mesh & operator=(const mesh & other) = delete;
	};

	deserializer & operator>>(deserializer & deserializer, mesh & mesh);
	serializer & operator<<(serializer & serializer, const mesh & mesh);
}

#endif
