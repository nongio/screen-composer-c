#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include "screencomposer.h"
#include "screencomposer_backend.h"
#include "sc_output.h"

void compositor_backend_on_new_output(struct wl_listener *listener, void *data)
{
	struct sc_compositor *compositor =
		wl_container_of(listener, compositor, new_output);
	struct wlr_output *wlr_output = data;


	struct sc_output *output = sc_output_create(wlr_output, compositor);

	wl_list_insert(&compositor->outputs, &output->link);
	wlr_output_layout_add_auto(compositor->output_layout, wlr_output);

}

