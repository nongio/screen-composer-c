#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "sc_workspace.h"

struct sc_workspace *
sc_workspace_create()
{
	struct sc_workspace *workspace = calloc(1, sizeof(struct sc_workspace));
	wl_list_init(&workspace->views_toplevel);
	wl_list_init(&workspace->layers_overlay);
	wl_list_init(&workspace->layers_top);
	wl_list_init(&workspace->layers_bottom);
	wl_list_init(&workspace->layers_background);
	wl_list_init(&workspace->sc_layers);

	return workspace;
}
