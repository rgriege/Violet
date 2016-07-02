#ifndef VIOLET_TEXTURE_H
#define VIOLET_TEXTURE_H

#include "violet/core/types.h"

typedef struct vlt_mesh vlt_mesh;


typedef struct vlt_texture
{
	u32 handle;
	u32 width;
	u32 height;
} vlt_texture;

b8 vlt_load_png(vlt_texture *tex, const char *filename);
void vlt_texture_init(vlt_texture *tex, u32 w, u32 h, u32 fmt, void *data);
void vlt_texture_destroy(vlt_texture *tex);

void vlt_texture_coords_from_poly(vlt_mesh *tex_coords,
                                  const v2f *v, u32 n);

void vlt_texture_bind(const vlt_texture *tex);
void vlt_texture_unbind();

#endif
