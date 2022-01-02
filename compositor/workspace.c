#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_workspace.h"
#include "sc_workspace.h"

void
sc_compositor_setup_workspaces(struct sc_compositor *compositor)
{
	wl_list_init(&compositor->workspaces);

	struct sc_workspace *workspace = sc_workspace_create();
	compositor->current_workspace = workspace;

	wl_list_insert(&compositor->workspaces, &workspace->link);
}

void
sc_compositor_add_toplevel(struct sc_compositor *compositor,
						   struct sc_toplevel_view *view)
{
	wl_list_insert(&compositor->current_workspace->views_toplevel, &view->link);
}
