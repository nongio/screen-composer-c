#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/util/region.h>

#include "sc_compositor.h"
#include "sc_output.h"

void sc_render_output(struct sc_output *output, struct timespec *when,
		pixman_region32_t *damage) {
	struct wlr_output *wlr_output = output->wlr_output;
	struct wlr_renderer *renderer = output->compositor->wlr_renderer;

	wlr_renderer_begin(renderer, wlr_output->width, wlr_output->height);

	if (!pixman_region32_not_empty(damage)) {
		// Output isn't damaged but needs buffer swap
		goto renderer_end;
	}

	float clear_color[] = {0.25f, 0.25f, 0.25f, 1.0f};
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT);


renderer_end:
	wlr_renderer_scissor(renderer, NULL);
	wlr_output_render_software_cursors(wlr_output, damage);
	wlr_renderer_end(renderer);

	int width, height;
	wlr_output_transformed_resolution(wlr_output, &width, &height);

	pixman_region32_t frame_damage;
	pixman_region32_init(&frame_damage);

	enum wl_output_transform transform =
		wlr_output_transform_invert(wlr_output->transform);
	wlr_region_transform(&frame_damage, &output->damage->current,
		transform, width, height);

	wlr_output_set_damage(wlr_output, &frame_damage);
	pixman_region32_fini(&frame_damage);

	if (!wlr_output_commit(wlr_output)) {
		return;
	}
	output->last_frame = *when;
}


