#ifndef _SCREENCOMPOSER_BACKEND_H
#define _SCREENCOMPOSER_BACKEND_H

void sc_compositor_setup_backend(struct sc_compositor *compositor);

void compositor_backend_on_new_output(struct wl_listener *listener, void *data);

void compositor_backend_on_new_input(struct wl_listener *listener, void *data);

#endif
