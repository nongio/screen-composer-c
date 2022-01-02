#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/interfaces/wlr_output.h>
#include <wlr/types/wlr_output_damage.h>

#include "log.h"
#include "sc_output.h"
#include "sc_output_repaintdelay.h"
#include "sc_compositor_renderer.h"

static int output_repaint_timer_handler(void *data);

static void output_on_frame(struct wl_listener *listener, void *data);

static void output_on_present(struct wl_listener *listener, void *data);

struct sc_output *
sc_output_create(struct wlr_output *wlr_output,
				 struct sc_compositor *compositor)
{
	struct sc_output *output = calloc(1, sizeof(struct sc_output));
	/* TODO should check in the config if can be enabled */
	output->wlr_output = wlr_output;
	output->compositor = compositor;
	output->damage = wlr_output_damage_create(wlr_output);

	wlr_output_init_render(output->wlr_output, compositor->wlr_allocator,
						   compositor->wlr_renderer);

	if (!wl_list_empty(&output->wlr_output->modes)) {
		struct wlr_output_mode *mode =
			wlr_output_preferred_mode(output->wlr_output);
		wlr_output_set_mode(output->wlr_output, mode);
		wlr_output_enable(output->wlr_output, true);

		if (!wlr_output_commit(output->wlr_output)) {
			return NULL;
		}
	}
	output->enabled = true;

	output->projection_matrix = calloc(1, sizeof(float) * 9);

	output->repaint_timer = wl_event_loop_add_timer(
		compositor->wl_event_loop, output_repaint_timer_handler, output);

	/* Sets up a listener for the frame notify event. */
	output->on_frame.notify = output_on_frame;
	wl_signal_add(&output->damage->events.frame, &output->on_frame);

	output->on_present.notify = output_on_present;
	wl_signal_add(&output->wlr_output->events.present, &output->on_present);
	wlr_output_damage_whole(output->wlr_output);

	return output;
}

static int
output_repaint_timer_handler(void *data)
{
	LOG("output_repaint_timer_handler\n");
	struct sc_output *output = (struct sc_output *) data;
	/* Checks if there is a need to render or skip */

	if (output->wlr_output == NULL) {
		return 0;
	}

	output->wlr_output->frame_pending = false;

	bool needs_frame;
	pixman_region32_t damage;
	pixman_region32_init(&damage);
	if (!wlr_output_damage_attach_render(output->damage, &needs_frame,
										 &damage)) {
		return 0;
	}

	if (needs_frame) {
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);

		sc_render_output(output, &now, &damage);
	} else {
		wlr_output_rollback(output->wlr_output);
	}

	pixman_region32_fini(&damage);

	return 0;
}

static void
output_on_present(struct wl_listener *listener, void *data)
{
	struct sc_output *output = wl_container_of(listener, output, on_frame);
	struct wlr_output_event_present *output_event = data;

	if (!output->enabled || !output_event->presented) {
		return;
	}
	sc_output_update_presentation(output, output_event);
}

static void
output_on_frame(struct wl_listener *listener, void *data)
{
	struct sc_output *output = wl_container_of(listener, output, on_frame);

	if (!output->enabled || !output->wlr_output->enabled) {
		return;
	}

	int delay = sc_output_get_ms_until_refresh(output);
	LOG("output_on_frame delay: %d\n", delay);

	// If the delay is less than 1 millisecond (which is the least we can wait)
	// then just render right away.
	if (delay < 1) {
		output_repaint_timer_handler(output);
	} else {
		output->wlr_output->frame_pending = true;
		wl_event_source_timer_update(output->repaint_timer, delay);
	}

	// Send frame done to all visible surfaces
	// struct send_frame_done_data data = {0};
	// clock_gettime(CLOCK_MONOTONIC, &data.when);
	// data.msec_until_refresh = msec_until_refresh;
	// send_frame_done(output, &data);
}

// struct send_frame_done_data {
//	struct timespec when;
//	int msec_until_refresh;
// };

// static void send_frame_done_iterator(struct sc_output *output,
//		struct sc_view *view, struct wlr_surface *surface,
//		struct wlr_box *box, void *user_data) {
//	int view_max_render_time = 0;
//	if (view != NULL) {
//		view_max_render_time = view->max_render_time;
//	}
//
//	struct send_frame_done_data *data = user_data;
//
//	int delay = data->msec_until_refresh - output->max_render_time
//			- view_max_render_time;
//
//	if (output->max_render_time == 0 || view_max_render_time == 0 || delay < 1)
//{ 		wlr_surface_send_frame_done(surface, &data->when); 	} else { 		struct
//sway_surface *sway_surface = surface->data;
//		wl_event_source_timer_update(sway_surface->frame_done_timer, delay);
//	}
// }
//
// static void output_for_each_surface_iterator(struct wlr_surface *surface,
//		int sx, int sy, void *_data) {
//	struct surface_iterator_data *data = _data;
//
//	struct wlr_box box;
//	bool intersects = get_surface_box(data, surface, sx, sy, &box);
//	if (!intersects) {
//		return;
//	}
//
//	data->user_iterator(data->output, data->view, surface, &box,
//		data->user_data);
// }

// static void sc_output_send_frame_done(struct sc_output *output, struct
// send_frame_done_data *data) {
//	//output_for_each_surface(output, send_frame_done_iterator, data);
// }
