#ifndef _SC_COMPOSITOR_H
#define _SC_COMPOSITOR_H

#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/gles2.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_presentation_time.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>

#include "sc-layer-shell.h"

enum sc_cursor_mode {
	SC_CURSOR_PASSTHROUGH,
	SC_CURSOR_MOVE,
	SC_CURSOR_RESIZE,
};

struct sc_compositor {
	struct wl_display *wl_display;
	struct wl_event_loop *wl_event_loop;
	struct wlr_backend *wlr_backend;
	struct wlr_renderer *wlr_renderer;
	struct wlr_allocator *wlr_allocator;
	struct wlr_compositor *wlr_compositor;
	struct wlr_presentation *wlr_presentation;
	struct wlr_xdg_output_manager_v1 *output_manager;
	struct wlr_egl *egl;

	struct sc_workspace *current_workspace;
	struct sc_view *current_view;
	struct wl_list workspaces;

	/* seat */
	struct wlr_seat *seat;

	/* protocols */
	struct wlr_xdg_shell *xdg_shell;
	struct wlr_layer_shell_v1 *layer_shell;
	struct sc_layer_shell_v1 *layer_composer_shell;
	/* inputs */
	struct wl_list keyboards;
	struct wlr_cursor *cursor;
	struct wlr_xcursor_manager *cursor_mgr;

	/* view grab */
	enum sc_cursor_mode cursor_mode;
	struct sc_toplevel_view *grabbed_view;
	double grab_x, grab_y;
	struct wlr_box grab_box;
	uint32_t resize_edges;

	/* output */
	struct wl_list outputs;
	struct wlr_output_layout *output_layout;

	/* listeners */
	struct wl_listener on_new_output;
	struct wl_listener on_new_input;

	struct wl_listener on_cursor_motion;
	struct wl_listener on_cursor_motion_absolute;
	struct wl_listener on_cursor_button;
	struct wl_listener on_cursor_axis;
	struct wl_listener on_cursor_frame;

	struct wl_listener on_request_set_cursor;
	struct wl_listener on_request_set_selection;

	struct wl_listener on_new_xdg_surface;
	struct wl_listener on_new_layer_surface;
	struct wl_listener on_new_layercomposer_surface;
};

struct sc_compositor* sc_compositor_create();
void sc_compositor_destroy();
void sc_compositor_start_server();
void sc_compositor_start_eventloop();
char* sc_compositor_get_socket();
struct sc_output *sc_compositor_output_at(double lx, double ly);

struct sc_view *sc_composer_view_at(struct sc_compositor *compositor, double x, double
		y, struct wlr_surface **surface, double *sx, double *sy);
#endif
