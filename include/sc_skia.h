#ifndef _SC_SKIA_C_H
#define _SC_SKIA_C_H
#include "sc_fbo.h"

struct sc_texture_attributes {
	GLenum target;
	GLuint tex;
	GLenum format;
	unsigned int width;
	unsigned int height;
};

struct skia_context *skia_context_create_for_view(struct sc_fbo *fbo);
void skia_draw(struct skia_context *skia);
void skia_submit(struct skia_context *skia);

struct skia_image *skia_image_from_texture(struct skia_context *skia, struct wlr_surface *surface, struct sc_texture_attributes *texture_attributes);
void free_skia_image(struct wlr_surface *surface);

void skia_draw_surface(struct skia_context *skia, struct wlr_surface *surface, int x, int y, int w, int h);

#endif