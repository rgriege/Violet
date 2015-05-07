// ============================================================================

#include "engine/graphics/texture/Texture.h"

#include <GL/glew.h>

using namespace Violet;

// ============================================================================

void Texture::bind(const Texture & texture)
{
	glBindTexture(GL_TEXTURE_2D, texture.m_handle);
}

// ----------------------------------------------------------------------------

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ============================================================================

Texture::Texture(const uint32 width, const uint32 height, const void * const data, const uint32 format) :
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

// ============================================================================
