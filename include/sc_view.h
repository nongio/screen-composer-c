#ifndef _SC_VIEW_H
#define _SC_VIEW_H

#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>

#include "sc_output.h"

struct sc_view_impl {
	void (*for_each_surface)(struct sc_view *view,
							 wlr_surface_iterator_func_t iterator,
							 void *user_data);
	void (*for_each_popup_surface)(struct sc_view *view,
								   wlr_surface_iterator_func_t iterator,
								   void *user_data);
	struct wlr_surface *(*surface_at)(struct sc_view *view, double x, double y,
									  double *sx, double *sy);
	void (*focus)(struct sc_view *view);
	void (*activate)(struct sc_view *view);
	void (*deactivate)(struct sc_view *view);
	void (*commit)(struct sc_view *view);
};

struct sc_view {
	struct wl_list link;
	const struct sc_view_impl *impl;
	// protocol surfaces
	struct wlr_surface *surface;
	struct wlr_subsurface *subsurface;

	struct sc_compositor *compositor;
	struct sc_output *output;

	// states
	bool mapped;

	// position / size
	struct wlr_box frame;
	float *matrix;

	struct sc_view *parent;
	struct wl_list children;

	int max_render_time; // In milliseconds

	struct wl_listener on_surface_commit;
	struct wl_listener on_subsurface_new;
	struct wl_listener on_subview_destroy;
};

void sc_view_init(struct sc_view *view, struct sc_view_impl *impl,
				  struct wlr_surface *surface);

void sc_view_map(struct sc_view *view);

void sc_view_unmap(struct sc_view *view);

void sc_view_destroy(struct sc_view *view);

void sc_view_damage_whole(struct sc_view *view);

void sc_view_for_each_surface(struct sc_view *view,
							  wlr_surface_iterator_func_t iterator,
							  void *user_data);

bool sc_view_is_visible(struct sc_view *view);

struct wlr_surface *sc_view_surface_at(struct sc_view *view, double x, double y,
									   double *sx, double *sy);

void sc_view_set_output(struct sc_view *view, struct sc_output *output);

void sc_view_focus(struct sc_view *view);

void sc_view_activate(struct sc_view *view);

void sc_view_deactivate(struct sc_view *view);

#endif
