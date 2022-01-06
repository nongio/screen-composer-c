#ifndef _SC_COMPOSITOR_RENDERER_H
#define _SC_COMPOSITOR_RENDERER_H

struct sc_output;

void sc_compositor_setup_gles2();

void sc_render_output(struct sc_output *output, struct timespec *when,
		pixman_region32_t *damage);

#endif
