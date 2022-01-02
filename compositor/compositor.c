#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_backend.h"
#include "sc_compositor_cursor.h"

static struct sc_compositor *compositor = NULL;
static char *socket = "";

struct sc_compositor *
sc_compositor_create()
{
	compositor = calloc(1, sizeof(struct sc_compositor));

	DLOG("screen-composer init.\n");

	wl_list_init(&compositor->outputs);

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
	//	wlr_data_device_manager_create(compositor->wl_display);

	compositor->wlr_presentation = wlr_presentation_create(
		compositor->wl_display, compositor->wlr_backend);

	compositor->output_layout = wlr_output_layout_create();

	compositor->egl =
		(struct wlr_egl *) wlr_gles2_renderer_get_egl(compositor->wlr_renderer);

	/* Configures a single seat */
	compositor->seat = wlr_seat_create(compositor->wl_display, "seat0");

	sc_compositor_setup_cursor(compositor);
	sc_compositor_setup_backend(compositor);

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
	DLOG("sc_compositor_start\n");
	const char *s = wl_display_add_socket_auto(compositor->wl_display);
	socket = strdup(s);

	if (!socket) {
		LOG("error: unable to create socket\n");
		wlr_backend_destroy(compositor->wlr_backend);
		return;
	}

	/* Start the backend. This will enumerate outputs and inputs, become the DRM
	 * master, etc */
	if (!wlr_backend_start(compositor->wlr_backend)) {
		LOG("error: unable to start backend\n");
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
