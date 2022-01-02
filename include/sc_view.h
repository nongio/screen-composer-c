#ifndef _SC_VIEW_H
#define _SC_VIEW_H

#include <wlr/types/wlr_xdg_shell.h>

struct sc_view {
  // protocol surfaces
  struct wlr_surface *surface;
  struct wlr_subsurface * subsurface;

  // states
  bool mapped;
  bool moving;
  bool resizing;

  // position / size
  float *matrix;

  struct wl_list children;
};

struct sc_view *sc_view_create(struct wlr_surface *surface,
							   struct sc_view *parent);

#endif

