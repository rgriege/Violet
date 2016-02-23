#ifndef VIOLET_RENDER_COMPONENT_DATA_H
#define VIOLET_RENDER_COMPONENT_DATA_H

#include <memory>

#include "violet/graphics/mesh.h"

namespace vlt
{
	struct Deserializer;
	struct Serializer;
	struct Shader_Program;

	struct VIOLET_API Render_Component_Data
	{
		u32 vertex_array_buffer;
		std::unique_ptr<Mesh> mesh;
		std::shared_ptr<Shader_Program> shader;

	protected:

		Render_Component_Data(Deserializer & deserializer);
		Render_Component_Data(const Poly & poly, std::shared_ptr<Shader_Program> shader);
		Render_Component_Data(const Render_Component_Data &);
		Render_Component_Data & operator=(const Render_Component_Data &) = delete;
		Render_Component_Data(Render_Component_Data && other);
		~Render_Component_Data();
	};

	Deserializer & operator>>(Deserializer & deserializer, Render_Component_Data & component);
	Serializer & operator<<(Serializer & serializer, const Render_Component_Data & component);
}

#endif
