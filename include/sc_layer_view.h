#ifndef _SC_LAYER_VIEW_H
#define _SC_LAYER_VIEW_H

#include <wlr/types/wlr_layer_shell_v1.h>

#include "sc_compositor.h"
#include "sc_view.h"

struct sc_layer_view {
	struct sc_view super;
	struct wl_list link;

	/* protocol surfaces */
	struct wlr_layer_surface_v1 *layer_surface;

	/* listeners */
	struct wl_listener on_map;
	struct wl_listener on_unmap;
	struct wl_listener on_destroy;
	struct wl_listener on_surface_commit;
};

struct sc_layer_view *
sc_layer_view_create(struct wlr_layer_surface_v1 *layer_surface,
						struct sc_compositor *compositor);

#endif

