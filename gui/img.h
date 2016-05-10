#ifndef VIOLET_IMG_H
#define VIOLET_IMG_H

#include "violet/core/types.h"

typedef struct vlt_img vlt_img;
typedef struct vlt_shader_program vlt_shader_program;

vlt_img * vlt_img_create();
void vlt_img_free(vlt_img * img);
b8 vlt_img_load(vlt_img * img, const char * filename);
void vlt_img_destroy(vlt_img * img);

void vlt_img_render(vlt_img * img, s32 x, s32 y, vlt_shader_program * p);

#endif
