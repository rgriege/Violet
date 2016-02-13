#ifndef VIOLET_TEXTURE_COMPONENT_H
#define VIOLET_TEXTURE_COMPONENT_H

#include "violet/component/component.h"
#include "violet/core/handle.h"
#include "violet/graphics/component/render_component_data.h"
#include "violet/task/thread.h"

#include "violet/graphics/texture/texture.h"

namespace vlt
{
	struct component_deserializer;
	struct serializer;
	struct shader_program;

	struct VIOLET_API texture_component final : public component_base<texture_component, 0>, public render_component_data
	{
		std::shared_ptr<texture> m_texture;
		std::unique_ptr<mesh> m_texCoords;

		static tag get_tag_static();
		static thread get_thread_static();

		texture_component(handle entity_id, component_deserializer & deserializer);
		texture_component(handle entity_id, const poly & p, std::shared_ptr<shader_program> shader, std::shared_ptr<texture> texture, const poly & texCoords);
		texture_component(const texture_component &) = delete;
		texture_component & operator=(const texture_component &) = delete;
		texture_component(texture_component && other);
		~texture_component();
	};

	VIOLET_API component_deserializer & operator>>(component_deserializer & deserializer, texture_component & component);
	VIOLET_API serializer & operator<<(serializer & serializer, const texture_component & component);
}

#endif