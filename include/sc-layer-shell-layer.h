#ifndef _SC_LAYER_SHELL_LAYER_H
#define _SC_LAYER_SHELL_LAYER_H

#include <time.h>

#include "sc-layer-shell.h"

void
shell_handle_get_layer_surface(struct wl_client   *wl_client,
				     struct wl_resource *client_resource,
				     uint32_t		 id,
				     struct wl_resource *surface_resource,
				     struct wl_resource *output_resource);

void
layer_update_animations(struct sc_layer_surface_v1 *layer, double timestamp);
#endif

