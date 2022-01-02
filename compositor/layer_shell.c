#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_layer_view.h"
#include "sc_view.h"

static void
compositor_layer_surface_new(struct wl_listener *listener, void *data)
{

	LOG("compositor_new_layer_surface\n");

	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_new_layer_surface);

	struct wlr_layer_surface_v1 *layer_surface = data;

	sc_layer_view_create(layer_surface, compositor);
}

void
sc_compositor_setup_layershell(struct sc_compositor *compositor)
{
	compositor->layer_shell = wlr_layer_shell_v1_create(compositor->wl_display);

	compositor->on_new_layer_surface.notify = compositor_layer_surface_new;

	wl_signal_add(&compositor->layer_shell->events.new_surface,
				  &compositor->on_new_layer_surface);
}
