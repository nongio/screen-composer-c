#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/util/region.h>

#include "log.h"
#include "sc_output.h"
#include "sc_view.h"
#include "sc_workspace.h"



/* this function iterates across all views intersecting the output */
void
sc_output_for_each_view_surface(struct sc_output *output,
								wlr_surface_iterator_func_t surface_iterator,
								void *data)
{
	struct sc_view *toplevel;
	struct sc_workspace *workspace = output->compositor->current_workspace;
	wl_list_for_each (toplevel, &workspace->views_toplevel, link) {
		if (sc_output_intersect_view(output, toplevel) == false) {
			return;
		}
		sc_view_for_each_surface(toplevel, surface_iterator, data);
	}
}
