// ============================================================================

#include "violet/graphics/texture/texture.h"

#include "violet/utility/Buffer.h"

#include <cstring>
#include <GL/glew.h>
#include <png.h>

using namespace vlt;

// ============================================================================

std::unique_ptr<texture> texture::load_png(const char * filename)
{
	png_image image;
	memset(&image, 0, (sizeof image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename) != 0)
	{
		heap_buffer<png_byte> buffer{ PNG_IMAGE_SIZE(image) };
		image.format = PNG_FORMAT_RGBA;
		if (buffer != NULL && png_image_finish_read(&image, NULL, buffer, 0, NULL) != 0)
			return std::unique_ptr<texture>(new texture(filename, image.width, image.height, buffer, GL_RGBA));
	}

	return nullptr;
}

// ----------------------------------------------------------------------------

void texture::bind(const texture & texture)
{
	glBindTexture(GL_TEXTURE_2D, texture.m_handle);
}

// ----------------------------------------------------------------------------

void texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ----------------------------------------------------------------------------

texture::Cache & texture::get_cache()
{
	static Cache s_cache(&texture::load_png);
	return s_cache;
}

// ============================================================================

texture::texture(std::string filename, const u32 width, const u32 height, const void * const data, const u32 format) :
	m_filename(std::move(filename)),
	m_handle(0)
{
	glGenTextures(1, &m_handle);
	bind(*this);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unbind();
}

// ----------------------------------------------------------------------------

texture::texture(texture && other) :
	m_handle(other.m_handle)
{
	other.m_handle = 0;
}

// ----------------------------------------------------------------------------

texture::~texture()
{
	if (m_handle != 0)
		glDeleteTextures(1, &m_handle);
}

// ----------------------------------------------------------------------------

std::string const & texture::get_name() const
{
	return m_filename;
}

// ============================================================================
