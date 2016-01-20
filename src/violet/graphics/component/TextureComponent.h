#ifndef VIOLET_TextureComponent_H
#define VIOLET_TextureComponent_H

#include "violet/component/Component.h"
#include "violet/graphics/component/RenderComponentData.h"
#include "violet/task/Thread.h"

#include "violet/graphics/texture/Texture.h"

namespace Violet
{
	class ComponentDeserializer;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API TextureComponent : public ComponentBase<TextureComponent, 0>, public RenderComponentData
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		TextureComponent(EntityId entityId, ComponentDeserializer & deserializer);
		TextureComponent(EntityId entityId, const Polygon & poly, std::shared_ptr<ShaderProgram> shader, std::shared_ptr<Texture> texture, const Polygon & texCoords);
		TextureComponent(TextureComponent && other);
		~TextureComponent();

	private:

		TextureComponent(const TextureComponent &) = delete;
		TextureComponent & operator=(const TextureComponent &) = delete;

	public:

		std::shared_ptr<Texture> m_texture;
		std::unique_ptr<Mesh> m_texCoords;
	};

	VIOLET_API ComponentDeserializer & operator>>(ComponentDeserializer & deserializer, TextureComponent & component);
	VIOLET_API Serializer & operator<<(Serializer & serializer, const TextureComponent & component);
}

#endif