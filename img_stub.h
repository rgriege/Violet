#ifndef VIOLET_IMG_STUB_H
#define VIOLET_IMG_STUB_H

typedef struct gui_img
{
	u32 handle;
	u32 width;
	u32 height;
	u32 blend;
} gui_img_t;

typedef enum gui_blend
{
	GUI_BLEND_NRM,
	GUI_BLEND_ADD,
	GUI_BLEND_MUL,
	GUI_BLEND__COUNT
} gui_blend_e;

b32  img_load(gui_img_t *img, const char *filename);
void img_destroy(gui_img_t *img);

#endif // VIOLET_IMG_STUB_H

#ifdef IMG_STUB_IMPLEMENTATION

b32 img_load(gui_img_t *img, const char *filename)
{
	b32 ret = false;
	int w, h;
	u8 *image = stbi_load(filename, &w, &h, NULL, 4);
	if (image) {
		img->handle = 42; /* don't use 0 - represents NULL in OpenGL */
		img->width  = w;
		img->height = h;
		img->blend  = 0;
		stbi_image_free(image);
		ret = true;
	} else {
		log_error("img_load(%s) error", filename);
	}
	return ret;
}

void img_destroy(gui_img_t *img)
{
	img->handle = 0;
}

#undef IMG_STUB_IMPLEMENTATION
#endif // IMG_STUB_IMPLEMENTATION
