#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/types/wlr_keyboard.h>

#include "sc_keyboard.h"

static void
keyboard_handle_modifiers(struct wl_listener *listener, void *data)
{

	struct sc_keyboard *keyboard =
		wl_container_of(listener, keyboard, on_modifiers);

	wlr_seat_set_keyboard(keyboard->compositor->seat, keyboard->device);

	/* Send modifiers to the client. */
	wlr_seat_keyboard_notify_modifiers(keyboard->compositor->seat,
									   &keyboard->device->keyboard->modifiers);
}

static bool
handle_keybinding(struct sc_compositor *compositor, xkb_keysym_t sym)
{

	switch (sym) {
	case XKB_KEY_q:
		wl_display_terminate(compositor->wl_display);
		break;
	case XKB_KEY_F1:
		/* Cycle to the next view */
		//		if (wl_list_length(&server->views) < 2) {
		//			break;
		//		}
		//		struct tinywl_view *next_view = wl_container_of(
		//			server->views.prev, next_view, link);
		//		focus_view(next_view, next_view->xdg_surface->surface);
		break;
	default:
		return false;
	}
	return true;
}

void
keyboard_handle_key(struct wl_listener *listener, void *data)
{
	/* This event is raised when a key is pressed or released. */
	struct sc_keyboard *keyboard = wl_container_of(listener, keyboard, on_key);

	struct sc_compositor *compositor = keyboard->compositor;
	struct wlr_event_keyboard_key *event = data;
	struct wlr_seat *seat = compositor->seat;

	/* Translate libinput keycode -> xkbcommon */
	uint32_t keycode = event->keycode + 8;
	/* Get a list of keysyms based on the keymap for this keyboard */
	const xkb_keysym_t *syms;
	int nsyms = xkb_state_key_get_syms(keyboard->device->keyboard->xkb_state,
									   keycode, &syms);

	bool handled = false;
	uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->device->keyboard);
	if ((modifiers & WLR_MODIFIER_ALT) &&
		event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
		/* If alt is held down and this button was _pressed_, we attempt to
		 * process it as a compositor keybinding. */
		for (int i = 0; i < nsyms; i++) {
			handled = handle_keybinding(compositor, syms[i]);
		}
	}

	if (!handled) {
		/* Otherwise, we pass it along to the client. */
		wlr_seat_set_keyboard(seat, keyboard->device);
		wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode,
									 event->state);
	}
}

struct sc_keyboard *
sc_keyboard_create(struct wlr_input_device *device,
									   struct sc_compositor *compositor)
{
	struct sc_keyboard *keyboard = calloc(1, sizeof(struct sc_keyboard));

	keyboard->compositor = compositor;
	/* We need to prepare an XKB keymap and assign it to the keyboard. This
	 * assumes the defaults (e.g. layout = "us"). */

	//	struct xkb_rule_names rules = { 0 };
	struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	struct xkb_keymap *keymap =
		xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);

	wlr_keyboard_set_keymap(device->keyboard, keymap);
	xkb_keymap_unref(keymap);
	xkb_context_unref(context);
	wlr_keyboard_set_repeat_info(device->keyboard, 25, 600);

	keyboard->device = device;

	keyboard->on_modifiers.notify = keyboard_handle_modifiers;
	wl_signal_add(&keyboard->device->keyboard->events.modifiers,
				  &keyboard->on_modifiers);

	keyboard->on_key.notify = keyboard_handle_key;
	wl_signal_add(&keyboard->device->keyboard->events.key, &keyboard->on_key);

	return keyboard;
}
