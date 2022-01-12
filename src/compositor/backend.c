#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_backend.h"
#include "sc_compositor_keyboard.h"
#include "sc_keyboard.h"
#include "sc_output.h"

void sc_compositor_setup_backend(struct sc_compositor *compositor)
{
	wl_list_init(&compositor->outputs);

	compositor->on_new_output.notify = compositor_backend_on_new_output;
	wl_signal_add(&compositor->wlr_backend->events.new_output,
				  &compositor->on_new_output);

	wl_list_init(&compositor->keyboards);

	compositor->on_new_input.notify = compositor_backend_on_new_input;
	wl_signal_add(&compositor->wlr_backend->events.new_input,
				  &compositor->on_new_input);
}

void
compositor_backend_on_new_output(struct wl_listener *listener, void *data)
{
	DLOG("[backend_on_new_output]\n");
	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_new_output);
	struct wlr_output *wlr_output = data;

	wlr_output_layout_add_auto(compositor->output_layout, wlr_output);

	struct sc_output *output = sc_output_create(wlr_output, compositor);

	wl_list_insert(&compositor->outputs, &output->link);
}

void
compositor_backend_on_new_input(struct wl_listener *listener, void *data)
{
	DLOG("[backend_on_new_input]\n");
	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_new_input);
	struct wlr_input_device *device = data;

	switch (device->type) {
	case WLR_INPUT_DEVICE_KEYBOARD:
		DLOG("new keyboard\n");
		wlr_seat_set_keyboard(compositor->seat, device);
		wl_list_insert(&compositor->keyboards,
					   &(sc_keyboard_create(device, compositor))->link);
		break;
	case WLR_INPUT_DEVICE_POINTER:
		DLOG("new pointer\n");
		wlr_cursor_attach_input_device(compositor->cursor, device);
		break;
	default:
		break;
	}
	/* We need to let the wlr_seat know what our capabilities are, which is
	 * communiciated to the client. In TinyWL we always have a cursor, even if
	 * there are no pointer devices, so we always include that capability. */
	uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
	if (!wl_list_empty(&compositor->keyboards)) {
		caps |= WL_SEAT_CAPABILITY_KEYBOARD;
	}
	wlr_seat_set_capabilities(compositor->seat, caps);
}
