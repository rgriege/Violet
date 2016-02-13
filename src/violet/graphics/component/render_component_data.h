#ifndef VIOLET_RENDER_COMPONENT_DATA_H
#define VIOLET_RENDER_COMPONENT_DATA_H

#include <memory>

#include "violet/graphics/mesh.h"

namespace vlt
{
	struct deserializer;
	struct serializer;
	struct shader_program;

	struct VIOLET_API render_component_data
	{
		u32 m_vertexArrayBuffer;
		std::unique_ptr<mesh> m_mesh;
		std::shared_ptr<shader_program> m_shader;

	protected:

		render_component_data(deserializer & deserializer);
		render_component_data(const poly & poly, std::shared_ptr<shader_program> shader);
		render_component_data(const render_component_data &) = delete;
		render_component_data & operator=(const render_component_data &) = delete;
		render_component_data(render_component_data && other);
		~render_component_data();
	};

	deserializer & operator>>(deserializer & deserializer, render_component_data & component);
	serializer & operator<<(serializer & serializer, const render_component_data & component);
}

#endif