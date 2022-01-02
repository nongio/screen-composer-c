#ifndef _SC_TOPLEVEL_VIEW_H
#define _SC_TOPLEVEL_VIEW_H

#include <wlr/types/wlr_xdg_shell.h>

#include "sc_compositor.h"
#include "sc_view.h"

struct sc_toplevel_view {
	struct wl_list link;
	struct sc_compositor *compositor;
	struct sc_view *view;
	/* protocol surfaces */
	struct wlr_xdg_surface *xdg_surface;

	/* listeners */
	struct wl_listener on_map;
	struct wl_listener on_unmap;
	struct wl_listener on_destroy;

	struct wl_listener on_request_move;
	struct wl_listener on_request_resize;
};

struct sc_toplevel_view *
sc_toplevel_view_create(struct wlr_xdg_surface *xdg_surface,
						struct sc_compositor *compositor);

#endif
