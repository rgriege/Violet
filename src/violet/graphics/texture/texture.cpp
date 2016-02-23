// ============================================================================

#include <cstring>
#include <GL/glew.h>
#include <png.h>

#include "violet/graphics/texture/texture.h"
#include "violet/utility/buffer.h"

using namespace vlt;

// ============================================================================

std::unique_ptr<Texture> Texture::load_png(const char * filename)
{
	png_image image;
	memset(&image, 0, (sizeof image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename) != 0)
	{
		Heap_Buffer<png_byte> buffer{ PNG_IMAGE_SIZE(image) };
		image.format = PNG_FORMAT_RGBA;
		if (buffer != NULL && png_image_finish_read(&image, NULL, buffer, 0, NULL) != 0)
			return std::make_unique<Texture>(filename, image.width, image.height, buffer, GL_RGBA);
	}

	return nullptr;
}

// ----------------------------------------------------------------------------

void Texture::bind(const Texture & texture)
{
	glBindTexture(GL_TEXTURE_2D, texture.handle);
}

// ----------------------------------------------------------------------------

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ----------------------------------------------------------------------------

Texture::Cache & Texture::get_cache()
{
	static Cache s_cache(&Texture::load_png);
	return s_cache;
}

// ============================================================================

Texture::Texture(std::string _filename, const u32 width, const u32 height, const void * const data, const u32 format) :
	filename(std::move(_filename)),
	handle(0)
{
	glGenTextures(1, &handle);
	bind(*this);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unbind();
}

// ----------------------------------------------------------------------------

Texture::Texture(Texture && other) :
	handle(other.handle)
{
	other.handle = 0;
}

// ----------------------------------------------------------------------------

Texture::~Texture()
{
	if (handle != 0)
		glDeleteTextures(1, &handle);
}

// ============================================================================

