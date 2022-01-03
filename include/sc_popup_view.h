#ifndef _SC_POPUP_VIEW_H
#define _SC_POPUP_VIEW_H

#include "sc_compositor.h"
#include "sc_view.h"

struct sc_popup_view {
	struct sc_view super;
	struct wl_list link;

	/* protocol surfaces */
	struct wlr_xdg_popup *xdg_popup;
	struct wlr_xdg_surface *xdg_surface;

	/* listeners */
	struct wl_listener on_map;
	struct wl_listener on_unmap;
	struct wl_listener on_destroy;
};

struct sc_popup_view *
sc_popup_view_create(struct wlr_xdg_popup *popup,
					 struct sc_view *parent);

#endif
