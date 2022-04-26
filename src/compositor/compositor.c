#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_xdg_output_v1.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/render/gles2.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_backend.h"
#include "sc_compositor_cursor.h"
#include "sc_compositor_layershell.h"
#include "sc_compositor_rendering.h"
#include "sc_compositor_seat.h"
#include "sc_compositor_workspace.h"
#include "sc_compositor_xdgshell.h"
#include "sc_compositor_layercompositor.h"
#include "sc_output.h"

extern struct sc_configuration configuration;

struct sc_compositor *compositor = NULL;
static char *socket = "";

struct sc_compositor *
sc_compositor_create()
{
	compositor = calloc(1, sizeof(struct sc_compositor));

	wl_list_init(&compositor->outputs);
	DLOG("screen-composer initializaton.\n");

	compositor->wl_display = wl_display_create();
	compositor->wl_event_loop =
		wl_display_get_event_loop(compositor->wl_display);

	compositor->wlr_backend = wlr_backend_autocreate(compositor->wl_display);

	compositor->wlr_renderer = wlr_renderer_autocreate(compositor->wlr_backend);
	wlr_renderer_init_wl_display(compositor->wlr_renderer,
								 compositor->wl_display);

	compositor->wlr_allocator = wlr_allocator_autocreate(
		compositor->wlr_backend, compositor->wlr_renderer);

	compositor->wlr_compositor =
		wlr_compositor_create(compositor->wl_display, compositor->wlr_renderer);

	compositor->wlr_presentation = wlr_presentation_create(
		compositor->wl_display, compositor->wlr_backend);

	compositor->output_layout = wlr_output_layout_create();

	compositor->egl =
		(struct wlr_egl *) wlr_gles2_renderer_get_egl(compositor->wlr_renderer);

	wlr_data_device_manager_create(compositor->wl_display);

	compositor->output_manager = wlr_xdg_output_manager_v1_create(
		compositor->wl_display, compositor->output_layout);
	
	sc_compositor_setup_seat(compositor);
	sc_compositor_setup_cursor(compositor);
	sc_compositor_setup_backend(compositor);
	sc_compositor_setup_workspaces(compositor);
	sc_compositor_setup_xdgshell(compositor);
	sc_compositor_setup_layershell(compositor);
	sc_compositor_setup_layercomposershell(compositor);

	// struct wlr_xdg_decoration_manager_v1 * decoration_manager =	
	// wlr_xdg_decoration_manager_v1_create(compositor->wl_display);
	// wlr_xdg_toplevel_decoration_v1_set_mode()

	sc_compositor_setup_gles2();

	return compositor;
}

void
sc_compositor_destroy()
{
	wl_display_destroy_clients(compositor->wl_display);
	wl_display_destroy(compositor->wl_display);
	free(compositor);
}

void
sc_compositor_start_server()
{
	DLOG("starting the server...\n");
	const char *s = wl_display_add_socket_auto(compositor->wl_display);
	socket = strdup(s);

	if (!socket) {
		ELOG("error: unable to create socket\n");
		wlr_backend_destroy(compositor->wlr_backend);
		return;
	}

	/* Start the backend. This will enumerate outputs and inputs, become the DRM
	 * master, etc */
	if (!wlr_backend_start(compositor->wlr_backend)) {
		ELOG("error: unable to start backend\n");
		wlr_backend_destroy(compositor->wlr_backend);
		wl_display_destroy(compositor->wl_display);
		return;
	}

	DLOG("backend started\n");
}
void
sc_compositor_start_eventloop()
{
	wl_display_run(compositor->wl_display);

	DLOG("finished running. bye.\n");
	sc_compositor_destroy();
}

char *
sc_compositor_get_socket()
{
	return socket;
}

struct sc_output *
sc_compositor_output_at(double lx, double ly)
{
	struct wlr_output *wlr_output =
		wlr_output_layout_output_at(compositor->output_layout, lx, ly);
	struct sc_output *output;
	wl_list_for_each (output, &compositor->outputs, link) {

		if (output->wlr_output == wlr_output) {
			return output;
		}
	}
	return NULL;
}
