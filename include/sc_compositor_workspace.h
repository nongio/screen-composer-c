#ifndef _SC_COMPOSITOR_WORKSPACE_H
#define _SC_COMPOSITOR_WORKSPACE_H

#include "sc_toplevel_view.h"

void sc_compositor_setup_workspaces(struct sc_compositor *compositor);
void sc_compositor_add_toplevel(struct sc_compositor *compositor,
								struct sc_toplevel_view *view);

#endif
