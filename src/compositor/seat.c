#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_data_device.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_seat.h"
#include "sc_layer_view.h"
#include "sc_toplevel_view.h"
#include "sc_view.h"

static void
request_set_cursor(struct wl_listener *listener, void *data)
{
	DLOG("request_set_cursor\n");
	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_request_set_cursor);

	struct wlr_seat_pointer_request_set_cursor_event *event = data;
	struct wlr_seat_client *focused_client =
		compositor->seat->pointer_state.focused_client;

	if (focused_client == event->seat_client) {

		wlr_cursor_set_surface(compositor->cursor, event->surface,
							   event->hotspot_x, event->hotspot_y);
	}
}

static void
request_set_selection(struct wl_listener *listener, void *data)
{
	DLOG("request_set_selection\n");
	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_request_set_selection);
	struct wlr_seat_request_set_selection_event *event = data;
	wlr_seat_set_selection(compositor->seat, event->source, event->serial);
}

void
sc_compositor_setup_seat(struct sc_compositor *compositor)
{

	/* Configures a single seat */
	compositor->seat = wlr_seat_create(compositor->wl_display, "seat0");

	compositor->on_request_set_cursor.notify = request_set_cursor;
	wl_signal_add(&compositor->seat->events.request_set_cursor,
				  &compositor->on_request_set_cursor);

	compositor->on_request_set_selection.notify = request_set_selection;
	wl_signal_add(&compositor->seat->events.request_set_selection,
				  &compositor->on_request_set_selection);
}
