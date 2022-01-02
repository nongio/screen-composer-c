#ifndef _SC_VIEW_H
#define _SC_VIEW_H

#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>

#include "sc_output.h"

struct sc_view {
	struct wl_list link;
	// protocol surfaces
	struct wlr_surface *surface;
	struct wlr_subsurface *subsurface;

	struct sc_output *output;


	// states
	bool mapped;

	// position / size
	struct wlr_box frame;
	float *matrix;

	struct sc_view *parent;
	struct wl_list children;

	struct wl_listener on_surface_commit;
	struct wl_listener on_subsurface_new;
	struct wl_listener on_subview_destroy;
};

void
sc_view_map(struct sc_view *view);

void
sc_view_destroy(struct sc_view *view);

void
sc_view_init(struct sc_view *view, struct wlr_surface *surface);
#endif
