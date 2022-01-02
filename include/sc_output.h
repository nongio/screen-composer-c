#ifndef _SC_OUTPUT_H
#define _SC_OUTPUT_H
#include <wlr/types/wlr_output_damage.h>

#include "sc_compositor.h"

struct sc_output {
	struct wl_list link;
	bool enabled;

	struct sc_compositor *compositor;
	struct wlr_output *wlr_output;
	struct wlr_output_layout *layout;
	struct wlr_output_damage *damage;
	float *projection_matrix;

	/* listeners */
	struct wl_listener on_frame;
	struct wl_listener on_present;

	/* repaint delay */
	struct timespec last_frame;
	struct timespec last_presentation;
	uint32_t refresh_nsec;
	int max_render_time; // In milliseconds
	struct wl_event_source *repaint_timer;
};

struct sc_output *sc_output_create(struct wlr_output *wlr_output,
					struct sc_compositor *compositor);

void
sc_output_on_present(struct wl_listener *listener, void *data);

#endif
