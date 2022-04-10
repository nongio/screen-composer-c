#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor_workspace.h"
#include "sc_popup_view.h"
#include "sc_view.h"

static void
xdg_popup_map(struct wl_listener *listener, void *data)
{

	LOG("xdg_popup_map\n");
	struct sc_popup_view *popup_view =
		wl_container_of(listener, popup_view, on_map);
	struct sc_view *view = (struct sc_view *) popup_view;
	sc_view_map(view);
	sc_view_damage_whole(view->parent);
}

static void
xdg_popup_unmap(struct wl_listener *listener, void *data)
{
	LOG("xdg_popup_unmap\n");
	struct sc_popup_view *popup_view =
		wl_container_of(listener, popup_view, on_unmap);
	struct sc_view *view = (struct sc_view *) popup_view;
	view->mapped = false;
	view->frame.x = popup_view->xdg_popup->geometry.x;
	view->frame.y = popup_view->xdg_popup->geometry.y;
	sc_view_damage_whole(view->parent);
}

static void
xdg_popup_destroy(struct wl_listener *listener, void *data)
{
	LOG("xdg_popup_destroy\n");
	struct sc_popup_view *popup_view =
		wl_container_of(listener, popup_view, on_destroy);

	struct sc_view *view = (struct sc_view *) popup_view;

	wl_list_remove(&view->link);
	wl_list_remove(&popup_view->on_map.link);
	wl_list_remove(&popup_view->on_unmap.link);
	wl_list_remove(&popup_view->on_destroy.link);

	free(popup_view);
}

void
sc_popup_unconstrain(struct sc_popup_view *popup_view)
{
	struct wlr_box *popup_box = &popup_view->xdg_popup->geometry;

	struct sc_view *view = (struct sc_view *) popup_view;
	struct wlr_output_layout *output_layout = view->parent->output->layout;
	struct wlr_output *wlr_output = wlr_output_layout_output_at(
		output_layout, view->parent->frame.x + popup_box->x,
		view->parent->frame.y + popup_box->y);

	struct wlr_box *output_box =
		wlr_output_layout_get_box(output_layout, wlr_output);

	struct sc_point p = {.x = 0, .y = 0};
	sc_view_get_absolute_position(view->parent, &p);

	struct wlr_box output_toplevel_box = {
		.x = output_box->x - p.x,
		.y = output_box->y - p.y,
		.width = output_box->width,
		.height = output_box->height,
	};
	wlr_xdg_popup_unconstrain_from_box(popup_view->xdg_popup,
									   &output_toplevel_box);
	double sx, sy;
	wlr_xdg_popup_get_position(popup_view->xdg_popup, &sx, &sy);

	view->frame.x = sx;
	view->frame.y = sy;
}

static void
popup_commit(struct sc_view *view)
{
	view->frame.width = view->surface->current.width;
	view->frame.height = view->surface->current.height;

	sc_output_add_damage_from_view(view->output, view, false);
}

static void
xdg_popup_popup_new(struct wl_listener *listener, void *data)
{
	LOG("xdg_popup_popup_new\n");
	struct sc_popup_view *popup_view =
		wl_container_of(listener, popup_view, on_new_popup);

	struct sc_view *view = (struct sc_view *) popup_view;

	struct wlr_xdg_popup *xdg_popup = data;
	struct sc_popup_view *subpopup_view = sc_popup_view_create(xdg_popup, view);

	struct sc_view *subview = (struct sc_view *) subpopup_view;

	wl_list_insert(&view->children, &subview->link);
}

static struct sc_view_impl popup_view_impl = {
	.commit = popup_commit,
};

struct sc_popup_view *
sc_popup_view_create(struct wlr_xdg_popup *xdg_popup, struct sc_view *parent)
{
	DLOG("sc_popup_view_create\n");
	struct sc_popup_view *popup_view = calloc(1, sizeof(struct sc_popup_view));

	popup_view->xdg_popup = xdg_popup;
	popup_view->xdg_surface = xdg_popup->base;

	struct sc_view *view = (struct sc_view *) popup_view;
	view->parent = parent;
	view->compositor = parent->compositor;
	view->output = parent->output;
	sc_view_init(view, SC_VIEW_POPUP, &popup_view_impl,
				 xdg_popup->base->surface);
	view->mapped = true;

	popup_view->on_map.notify = xdg_popup_map;
	wl_signal_add(&popup_view->xdg_surface->events.map, &popup_view->on_map);

	popup_view->on_unmap.notify = xdg_popup_unmap;
	wl_signal_add(&popup_view->xdg_surface->events.unmap,
				  &popup_view->on_unmap);

	popup_view->on_destroy.notify = xdg_popup_destroy;
	wl_signal_add(&popup_view->xdg_surface->events.destroy,
				  &popup_view->on_destroy);

	popup_view->on_new_popup.notify = xdg_popup_popup_new;
	wl_signal_add(&popup_view->xdg_surface->events.new_popup,
				  &popup_view->on_new_popup);

	sc_popup_unconstrain(popup_view);

	return popup_view;
}
