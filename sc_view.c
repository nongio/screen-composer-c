#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "sc_view.h"
#include "sc_output.h"
#include <wlr/types/wlr_surface.h>

void
sc_view_damage_part(struct sc_view *view)
{
	if (view->output != NULL) {
	}
}

void sc_view_damage_whole(struct sc_view *view)
{
	if (view->output != NULL) {
		sc_output_damage_view(view->output, view, true);
	}
}

void
sc_view_surface_commit_handler(struct wl_listener *listener, void *data)
{
	struct sc_view *view = wl_container_of(listener, view, on_surface_commit);

	//struct wlr_surface *surface = data;

	if (view->output == NULL) {
		return;
	}
	if (view->parent != NULL) {
		sc_output_damage_view(view->output, view->parent, false);
	} else {
		sc_output_damage_view(view->output, view, false);
	}
}
