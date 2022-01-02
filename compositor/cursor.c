#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_cursor.h"


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

	double sx = compositor->cursor->x;
	double sy = compositor->cursor->y;
	struct wlr_seat *seat = compositor->seat;
	//struct wlr_surface *surface = NULL;

	//struct tinywl_view *view = desktop_view_at(
	//	server, server->cursor->x, server->cursor->y, &surface, &sx, &sy);
	//if (!view) {
	//	/* If there's no view under the cursor, set the cursor image to a
	//	 * default. This is what makes the cursor image appear when you move it
	//	 * around the screen, not over any views. */
		wlr_xcursor_manager_set_cursor_image(compositor->cursor_mgr, "left_ptr",
											 compositor->cursor);
	//}
	//if (surface) {
	//	/*
	//	 * Send pointer enter and motion events.
	//	 *
	//	 * The enter event gives the surface "pointer focus", which is distinct
	//	 * from keyboard focus. You get pointer focus by moving the pointer over
	//	 * a window.
	//	 *
	//	 * Note that wlroots will avoid sending duplicate enter/motion events if
	//	 * the surface has already has pointer focus or if the client is already
	//	 * aware of the coordinates passed.
	//	 */
	//	wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
		wlr_seat_pointer_notify_motion(seat, time, sx, sy);
	//} else {
	//	/* Clear pointer focus so future button events and such are not sent to
	//	 * the last client to have the cursor over it. */
	//	wlr_seat_pointer_clear_focus(seat);
	//}
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

	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_cursor_button);
	struct wlr_event_pointer_button *event = data;

	wlr_seat_pointer_notify_button(compositor->seat, event->time_msec,
								   event->button, event->state);
	//double sx, sy;
	//struct wlr_surface *surface = NULL;
	//	struct tinywl_view *view = desktop_view_at(server,
	//			server->cursor->x, server->cursor->y, &surface, &sx, &sy);
	//	if (event->state == WLR_BUTTON_RELEASED) {
	//		/* If you released any buttons, we exit interactive move/resize mode.
	//*/ 		server->cursor_mode = TINYWL_CURSOR_PASSTHROUGH; 	} else {
	//		/* Focus that client if the button was _pressed_ */
	//		focus_view(view, surface);
	//	}
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
	LOG("[sc_compositor_setup_cursor]\n");

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
