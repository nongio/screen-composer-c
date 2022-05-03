#ifndef _SC_LAYER_COMPOSER_VIEW_H
#define _SC_LAYER_COMPOSER_VIEW_H


//#include "sc_compositor.h"
#include "sc_view.h"
//struct sc_view;
struct sc_compositor;

struct sc_layer_view {
	struct sc_view super;
	struct wl_list link;

	/* protocol surfaces */
	struct sc_layer_surface_v1 *layer_surface;

	/* listeners */
	struct wl_listener on_map;
	struct wl_listener on_unmap;
	struct wl_listener on_destroy;
	struct wl_listener on_surface_commit;
	struct wl_listener on_new_animation;
	struct wl_listener on_animation_update;
};

struct sc_layer_view *
sc_layer_view_create(struct sc_layer_surface_v1 *layer_surface,
						struct sc_compositor *compositor);

#endif

