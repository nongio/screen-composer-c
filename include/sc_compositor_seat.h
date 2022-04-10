#ifndef _SC_COMPOSITOR_SEAT_H
#define _SC_COMPOSITOR_SEAT_H

struct sc_compositor;
struct sc_toplevel_view;
struct sc_wlr_layer_view;
struct sc_view;

void sc_compositor_setup_seat(struct sc_compositor *compositor);

#endif

