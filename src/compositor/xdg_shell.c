#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_cursor.h"
#include "sc_toplevel_view.h"
#include "sc_view.h"

static void
xdg_toplevel_request_move(struct wl_listener *listener, void *data)
{

	struct sc_toplevel_view *toplevel_view =
		wl_container_of(listener, toplevel_view, on_request_move);
	struct sc_view *view = (struct sc_view *) toplevel_view;
	sc_compositor_begin_interactive(view->compositor, toplevel_view,
									SC_CURSOR_MOVE, 0);
}

static void
xdg_toplevel_request_resize(struct wl_listener *listener, void *data)
{

	struct wlr_xdg_toplevel_resize_event *event = data;

	struct sc_toplevel_view *toplevel_view =
		wl_container_of(listener, toplevel_view, on_request_resize);

	struct sc_view *view = (struct sc_view *) toplevel_view;

	sc_compositor_begin_interactive(view->compositor, toplevel_view,
									SC_CURSOR_RESIZE, event->edges);
}

static void
compositor_new_xdg_surface(struct wl_listener *listener, void *data)
{

	DLOG("compositor_new_xdg_surface\n");
	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, on_new_xdg_surface);
	struct wlr_xdg_surface *xdg_surface = data;

	if (xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL) {

		struct sc_toplevel_view *toplevel_view =
			sc_toplevel_view_create(xdg_surface, compositor);

		struct wlr_xdg_toplevel *toplevel = xdg_surface->toplevel;
		toplevel_view->on_request_move.notify = xdg_toplevel_request_move;

		wl_signal_add(&toplevel->events.request_move,
					  &toplevel_view->on_request_move);

		toplevel_view->on_request_resize.notify = xdg_toplevel_request_resize;
		wl_signal_add(&toplevel->events.request_resize,
					  &toplevel_view->on_request_resize);
	}
}

void
sc_compositor_setup_xdgshell(struct sc_compositor *compositor)
{
	compositor->xdg_shell = wlr_xdg_shell_create(compositor->wl_display);

	compositor->on_new_xdg_surface.notify = compositor_new_xdg_surface;

	wl_signal_add(&compositor->xdg_shell->events.new_surface,
				  &compositor->on_new_xdg_surface);
}
