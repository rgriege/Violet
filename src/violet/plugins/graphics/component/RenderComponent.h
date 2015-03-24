#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "violet/core/component/Component.h"

#include "violet/plugins/graphics/Mesh.h"
#include "violet/plugins/graphics/Color.h"
#include "violet/plugins/graphics/Config.h"

namespace Violet
{
	class Deserializer;
	class ShaderProgram;

	class VIOLET_GRAPHICS_API RenderComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		RenderComponent(const Entity & entity, Deserializer & deserializer);
		RenderComponent(RenderComponent && other);
		RenderComponent(const RenderComponent &) = delete;
		virtual ~RenderComponent() override;

	public:

		uint32 m_vertexArrayBuffer;
		Mesh m_mesh;
		Color m_color;
		std::shared_ptr<ShaderProgram> m_shader;
	};
}

#endif