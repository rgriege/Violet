// ============================================================================

#include "engine/graphics/texture/Texture.h"

#include "engine/utility/Buffer.h"

#include <GL/glew.h>
#include <png.h>

using namespace Violet;

// ============================================================================

std::unique_ptr<Texture> Texture::loadPng(const char * filename)
{
	png_image image;
	memset(&image, 0, (sizeof image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename) != 0)
	{
		HeapBuffer<png_byte> buffer{ PNG_IMAGE_SIZE(image) };
		image.format = PNG_FORMAT_RGBA;
		if (buffer != NULL && png_image_finish_read(&image, NULL, buffer, 0, NULL) != 0)
			return std::unique_ptr<Texture>(new Texture(filename, image.width, image.height, buffer, GL_RGBA));
	}

	return nullptr;
}

// ----------------------------------------------------------------------------

void Texture::bind(const Texture & texture)
{
	glBindTexture(GL_TEXTURE_2D, texture.m_handle);
}

// ----------------------------------------------------------------------------

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Cache & Texture::getCache()
{
	static Cache s_cache;
	return s_cache;
}

// ============================================================================

Texture::Texture(std::string filename, const uint32 width, const uint32 height, const void * const data, const uint32 format) :
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

Texture::Texture(Texture && other) :
	m_handle(other.m_handle)
{
	other.m_handle = 0;
}

// ----------------------------------------------------------------------------

Texture::~Texture()
{
	if (m_handle != 0)
		glDeleteTextures(1, &m_handle);
}

// ----------------------------------------------------------------------------

std::string const & Texture::getName() const
{
	return m_filename;
}

// ============================================================================
