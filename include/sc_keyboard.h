#ifndef _SC_KEYBOARD_H
#define _SC_KEYBOARD_H

#include <wlr/types/wlr_input_device.h>

#include "sc_compositor.h"

struct sc_keyboard {
	struct wl_list link;
	struct wlr_input_device *device;

	struct sc_compositor *compositor;

	/* listeners */
	struct wl_listener on_modifiers;
	struct wl_listener on_key;
};

struct sc_keyboard *sc_keyboard_create(struct wlr_input_device *device,
									   struct sc_compositor *compositor);

#endif
