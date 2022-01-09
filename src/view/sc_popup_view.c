#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor_workspace.h"
#include "sc_view.h"
#include "sc_popup_view.h"

static void
xdg_popup_map(struct wl_listener *listener, void *data)
{

	DLOG("xdg_popup_map\n");
	struct sc_popup_view *popup_view =
		wl_container_of(listener, popup_view, on_map);
	struct sc_view *view = (struct sc_view *) popup_view;

	//sc_view_map(view);
	sc_view_damage_whole(view->parent);
}

static void
xdg_popup_unmap(struct wl_listener *listener, void *data)
{
	DLOG("xdg_popup_unmap\n");
	struct sc_popup_view *popup_view =
		wl_container_of(listener, popup_view, on_unmap);
	struct sc_view *view = (struct sc_view *) popup_view;
	view->mapped = false;
	wl_list_remove(&popup_view->link);
	sc_view_damage_whole(view->parent);
}

static void
xdg_popup_destroy(struct wl_listener *listener, void *data)
{
	DLOG("xdg_popup_destroy\n");
	struct sc_popup_view *popup_view =
		wl_container_of(listener, popup_view, on_destroy);

	wl_list_remove(&popup_view->on_map.link);
	wl_list_remove(&popup_view->on_unmap.link);
	wl_list_remove(&popup_view->on_destroy.link);

	free(popup_view);
}

void sc_popup_unconstrain(struct sc_popup_view *popup_view)
{
	struct wlr_box *popup_box = &popup_view->xdg_popup->geometry;

	struct sc_view *view = (struct sc_view *) popup_view;
	struct wlr_output_layout *output_layout = view->parent->output->layout;
	struct wlr_output *wlr_output = wlr_output_layout_output_at(
		output_layout, view->parent->frame.x + popup_box->x,
		view->parent->frame.y + popup_box->y);

	struct wlr_box *output_box =
		wlr_output_layout_get_box(output_layout, wlr_output);

	struct wlr_box output_toplevel_box = {
		.x = output_box->x - view->parent->frame.x,
		.y = output_box->y - view->parent->frame.y,
		.width = output_box->width,
		.height = output_box->height,
	};
	wlr_xdg_popup_unconstrain_from_box(popup_view->xdg_popup,
									   &output_toplevel_box);
}

struct sc_popup_view *
sc_popup_view_create(struct wlr_xdg_popup *xdg_popup,
					 struct sc_view *parent)
{
	DLOG("sc_popup_view_create\n");
	struct sc_popup_view *popup_view =
		calloc(1, sizeof(struct sc_popup_view));

	popup_view->xdg_popup = xdg_popup;
	popup_view->xdg_surface = xdg_popup->base;

	struct sc_view *view = (struct sc_view *)popup_view;
	view->parent = parent;
	view->compositor = parent->compositor;
	sc_view_init(view, SC_VIEW_POPUP, NULL, xdg_popup->base->surface);

	popup_view->on_map.notify = xdg_popup_map;
	wl_signal_add(&popup_view->xdg_surface->events.map, &popup_view->on_map);

	popup_view->on_unmap.notify = xdg_popup_unmap;
	wl_signal_add(&popup_view->xdg_surface->events.unmap, &popup_view->on_unmap);

	popup_view->on_destroy.notify = xdg_popup_destroy;
	wl_signal_add(&popup_view->xdg_surface->events.destroy, &popup_view->on_destroy);

	sc_popup_unconstrain(popup_view);

	return popup_view;
}

