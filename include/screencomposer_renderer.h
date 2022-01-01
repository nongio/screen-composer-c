#ifndef _SCREENCOMPOSER_RENDERER_H
#define _SCREENCOMPOSER_RENDERER_H

void sc_render_output(struct sc_output *output, struct timespec *when,
		pixman_region32_t *damage);

#endif
