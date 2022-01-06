#ifndef _SC_GLES2_RENDERER_H
#define _SC_GLES2_RENDERER_H

void sc_renderer_load_shaders();

void sc_render_texture_with_output(struct wlr_gles2_texture_attribs *texture, int sx, int sy, int w,
			int h, enum wl_output_transform t, struct sc_output *output);

#endif
