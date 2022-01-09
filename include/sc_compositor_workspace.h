#ifndef _SC_COMPOSITOR_WORKSPACE_H
#define _SC_COMPOSITOR_WORKSPACE_H

struct sc_compositor;
struct sc_toplevel_view;
struct sc_layer_view;
struct sc_view;

void sc_compositor_setup_workspaces(struct sc_compositor *compositor);
void sc_compositor_add_toplevel(struct sc_compositor *compositor,
								struct sc_toplevel_view *view);
void sc_compositor_add_layer(struct sc_compositor *compositor,
								struct sc_layer_view *layer);

void
sc_composer_focus_view(struct sc_compositor *compositor, struct sc_view *view);
#endif
