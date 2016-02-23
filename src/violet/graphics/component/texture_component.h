#ifndef VIOLET_TEXTURE_COMPONENT_H
#define VIOLET_TEXTURE_COMPONENT_H

#include "violet/component/component_metadata.h"
#include "violet/core/handle.h"
#include "violet/graphics/component/render_component_data.h"
#include "violet/graphics/texture/texture.h"

namespace vlt
{
	struct Component_Deserializer;
	struct Serializer;
	struct Shader_Program;

	struct VIOLET_API Texture_Component final : public Render_Component_Data
	{
		std::shared_ptr<Texture> texture;
		std::unique_ptr<Mesh> tex_coords;

		static const Component_Metadata * metadata;

		Texture_Component(Handle entity_id, Component_Deserializer & deserializer);
		Texture_Component(Handle entity_id, const Poly & p, std::shared_ptr<Shader_Program> shader, std::shared_ptr<Texture> texture, const Poly & tex_coords);
		Texture_Component(const Texture_Component &) = delete;
		Texture_Component & operator=(const Texture_Component &) = delete;
		Texture_Component(Texture_Component && other);
	};

	VIOLET_API void install_texture_component();

	VIOLET_API Component_Deserializer & operator>>(Component_Deserializer & deserializer, Texture_Component & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const Texture_Component & component);
}

#endif
