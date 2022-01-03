#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor_workspace.h"
#include "sc_view.h"
#include "sc_toplevel_view.h"
#include "sc_popup_view.h"

static void
xdg_toplevel_map(struct wl_listener *listener, void *data)
{

	DLOG("xdg_toplevel_map\n");
	struct sc_toplevel_view *toplevel_view =
		wl_container_of(listener, toplevel_view, on_map);
	struct sc_view *view = (struct sc_view *) toplevel_view;

	sc_view_map(view);
	sc_compositor_add_toplevel(view->compositor, toplevel_view);
}

static void
xdg_toplevel_unmap(struct wl_listener *listener, void *data)
{
	DLOG("xdg_toplevel_unmap\n");
	struct sc_toplevel_view *toplevel_view =
		wl_container_of(listener, toplevel_view, on_unmap);
	struct sc_view *view = (struct sc_view *) toplevel_view;
	view->mapped = false;
	wl_list_remove(&toplevel_view->link);
}

static void
xdg_toplevel_destroy(struct wl_listener *listener, void *data)
{
	DLOG("xdg_toplevel_destroy\n");
	struct sc_toplevel_view *toplevel_view =
		wl_container_of(listener, toplevel_view, on_destroy);

	wl_list_remove(&toplevel_view->on_map.link);
	wl_list_remove(&toplevel_view->on_unmap.link);
	wl_list_remove(&toplevel_view->on_destroy.link);
	wl_list_remove(&toplevel_view->on_request_move.link);
	wl_list_remove(&toplevel_view->on_request_resize.link);

	free(toplevel_view);
}

static void
xdg_toplevel_popup_new(struct wl_listener *listener, void *data)
{
	DLOG("xdg_toplevel_popup_new\n");
	struct sc_toplevel_view *toplevel_view =
		wl_container_of(listener, toplevel_view, on_new_popup);
	struct sc_view *view = (struct sc_view *)toplevel_view;

	struct wlr_xdg_popup *xdg_popup = data;
	struct sc_popup_view *popup_view = sc_popup_view_create(xdg_popup,
			view);

	wl_list_insert(&view->children, &popup_view->link);
}

struct sc_toplevel_view *
sc_toplevel_view_create(struct wlr_xdg_surface *xdg_surface,
						struct sc_compositor *compositor)
{
	LOG("sc_toplevel_view_create\n");
	struct sc_toplevel_view *toplevel_view =
		calloc(1, sizeof(struct sc_toplevel_view));

	toplevel_view->xdg_surface = xdg_surface;

	struct sc_view *view = (struct sc_view *)toplevel_view;
	view->compositor = compositor;
	sc_view_init(view, xdg_surface->surface);

	toplevel_view->on_map.notify = xdg_toplevel_map;
	wl_signal_add(&xdg_surface->events.map, &toplevel_view->on_map);

	toplevel_view->on_unmap.notify = xdg_toplevel_unmap;
	wl_signal_add(&xdg_surface->events.unmap, &toplevel_view->on_unmap);

	toplevel_view->on_destroy.notify = xdg_toplevel_destroy;
	wl_signal_add(&xdg_surface->events.destroy, &toplevel_view->on_destroy);

	toplevel_view->on_new_popup.notify = xdg_toplevel_popup_new;
	wl_signal_add(&xdg_surface->events.new_popup, &toplevel_view->on_new_popup);

	return toplevel_view;
}
