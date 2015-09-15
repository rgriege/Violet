#ifndef VIOLET_TextureComponent_H
#define VIOLET_TextureComponent_H

#include "violet/component/Component.h"
#include "violet/graphics/component/RenderComponentData.h"

#include "violet/graphics/texture/Texture.h"

namespace Violet
{
	class Deserializer;
	class Serializer;
	class ShaderProgram;

	class VIOLET_API TextureComponent : public ComponentBase<TextureComponent>, public RenderComponentData
	{
	public:

		static Tag getStaticTag();

	public:

		TextureComponent(Entity & owner, Deserializer & deserializer);
		TextureComponent(Entity & owner, const Polygon & poly, std::shared_ptr<ShaderProgram> shader, std::shared_ptr<Texture> texture, const Polygon & texCoords);
		TextureComponent(TextureComponent && other);

	private:

		TextureComponent(const TextureComponent &) = delete;
		TextureComponent & operator=(const TextureComponent &) = delete;

	public:

		std::shared_ptr<Texture> m_texture;
		Mesh m_texCoords;
	};

	Deserializer & operator>>(Deserializer & deserializer, TextureComponent & component);
	Serializer & operator<<(Serializer & serializer, const TextureComponent & component);
}

#endif