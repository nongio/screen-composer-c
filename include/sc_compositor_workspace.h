#ifndef _SC_COMPOSITOR_WORKSPACE_H
#define _SC_COMPOSITOR_WORKSPACE_H

struct sc_compositor;
struct sc_toplevel_view;
struct sc_layer_view;

void sc_compositor_setup_workspaces(struct sc_compositor *compositor);
void sc_compositor_add_toplevel(struct sc_compositor *compositor,
								struct sc_toplevel_view *view);
void sc_compositor_add_layer(struct sc_compositor *compositor,
								struct sc_layer_view *layer);

#endif
