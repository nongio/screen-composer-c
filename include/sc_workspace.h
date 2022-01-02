#ifndef _SC_WORKSPACE_H
#define _SC_WORKSPACE_H

#include <wayland-server-core.h>

struct sc_workspace {
	struct wl_list link;

	struct wl_list views_toplevel;
	struct wl_list layers_overlay;
	struct wl_list layers_top;
	struct wl_list layers_bottom;
	struct wl_list layers_background;
};

struct sc_workspace *sc_workspace_create();
#endif


