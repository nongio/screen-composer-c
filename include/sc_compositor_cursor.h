#ifndef _SC_COMPOSITOR_CURSOR_H
#define _SC_COMPOSITOR_CURSOR_H

void sc_compositor_setup_cursor(struct sc_compositor *compositor);
void sc_compositor_begin_interactive(struct sc_compositor *compositor,
									 struct sc_toplevel_view *toplevel_view,
									 enum sc_cursor_mode mode, uint32_t edges);
#endif
