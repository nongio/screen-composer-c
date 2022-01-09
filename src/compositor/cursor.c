#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_data_device.h>

#include "log.h"
#include "sc_view.h"
#include "sc_output.h"
#include "sc_compositor.h"
#include "sc_compositor_cursor.h"
#include "sc_compositor_workspace.h"


static void
process_cursor_move(struct sc_compositor *compositor, uint32_t time)
{

}

static void
process_cursor_resize(struct sc_compositor *compositor, uint32_t time)
{

}

static void
process_cursor_motion(struct sc_compositor *compositor, uint32_t time)
{
	if (compositor->cursor_mode == SC_CURSOR_MOVE) {
		process_cursor_move(compositor, time);
		return;
	} else if (compositor->cursor_mode == SC_CURSOR_RESIZE) {
		process_cursor_resize(compositor, time);
		return;
	}

	double sx = 0;
	double sy = 0;
	struct wlr_seat *seat = compositor->seat;
	struct wlr_surface *surface = NULL;

	struct sc_view *view =
		sc_composer_view_at(compositor, compositor->cursor->x,
							compositor->cursor->y, &surface, &sx, &sy);

	if (!view) {
		/* If there's no view under the cursor, set the cursor image to a
		 * default. This is what makes the cursor image appear when you move it
		 * around the screen, not over any views. */
		wlr_xcursor_manager_set_cursor_image(compositor->cursor_mgr, "left_ptr",
											 compositor->cursor);
	}
	if (view != NULL) {
		bool focus_changed =
			wlr_seat_pointer_surface_has_focus(seat, surface) == false;

		if (focus_changed) {
			wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
		}
		wlr_seat_pointer_notify_motion(seat, time, sx, sy);
	} else {

		wlr_seat_pointer_clear_focus(seat);
	}
}

static void
compositor_cursor_motion(struct wl_listener *listener, void *data)
{

	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_cursor_motion);
	struct wlr_event_pointer_motion *event = data;

	wlr_cursor_move(compositor->cursor, event->device, event->delta_x,
					event->delta_y);

	process_cursor_motion(compositor, event->time_msec);
}

static void
compositor_cursor_motion_absolute(struct wl_listener *listener, void *data)
{

	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_cursor_motion_absolute);

	struct wlr_event_pointer_motion_absolute *event = data;
	wlr_cursor_warp_absolute(compositor->cursor, event->device, event->x,
							 event->y);

	process_cursor_motion(compositor, event->time_msec);
}

static void
compositor_cursor_button(struct wl_listener *listener, void *data)
{

	DLOG("wlr_seat_pointer_notify_button\n");
	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_cursor_button);
	struct wlr_event_pointer_button *event = data;

	wlr_seat_pointer_notify_button(compositor->seat, event->time_msec,
								   event->button, event->state);

	double sx = compositor->cursor->x;
	double sy = compositor->cursor->y;
	struct wlr_surface *surface = NULL;

	struct sc_view *view = sc_composer_view_at(compositor,
			compositor->cursor->x, compositor->cursor->y, &surface, &sx, &sy);

	if (event->state == WLR_BUTTON_RELEASED) {
		compositor->cursor_mode = SC_CURSOR_PASSTHROUGH;
	} else {
		sc_composer_focus_view(compositor, view);
	}

}

static void
compositor_cursor_axis(struct wl_listener *listener, void *data)
{

	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_cursor_axis);

	struct wlr_event_pointer_axis *event = data;

	wlr_seat_pointer_notify_axis(compositor->seat, event->time_msec,
								 event->orientation, event->delta,
								 event->delta_discrete, event->source);
}

static void
compositor_cursor_frame(struct wl_listener *listener, void *data)
{

	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_cursor_frame);

	wlr_seat_pointer_notify_frame(compositor->seat);
}


void
sc_compositor_setup_cursor(struct sc_compositor *compositor)
{
	DLOG("[sc_compositor_setup_cursor]\n");

	compositor->cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(compositor->cursor,
									compositor->output_layout);

	compositor->cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
	wlr_xcursor_manager_load(compositor->cursor_mgr, 1);
	wlr_xcursor_manager_set_cursor_image(compositor->cursor_mgr, "left_ptr",
										 compositor->cursor);

	compositor->on_cursor_motion.notify = compositor_cursor_motion;
	wl_signal_add(&compositor->cursor->events.motion,
				  &compositor->on_cursor_motion);

	compositor->on_cursor_motion_absolute.notify =
		compositor_cursor_motion_absolute;
	wl_signal_add(&compositor->cursor->events.motion_absolute,
				  &compositor->on_cursor_motion_absolute);

	compositor->on_cursor_button.notify = compositor_cursor_button;
	wl_signal_add(&compositor->cursor->events.button,
				  &compositor->on_cursor_button);

	compositor->on_cursor_axis.notify = compositor_cursor_axis;
	wl_signal_add(&compositor->cursor->events.axis,
				  &compositor->on_cursor_axis);

	compositor->on_cursor_frame.notify = compositor_cursor_frame;
	wl_signal_add(&compositor->cursor->events.frame,
				  &compositor->on_cursor_frame);

}
