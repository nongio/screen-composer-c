#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "sc_view.h"
#include <wlr/types/wlr_surface.h>

struct sc_view *
sc_view_create(struct wlr_surface *surface, struct sc_view *parent)
{
	struct sc_view *view = calloc(1, sizeof(struct sc_view));

	return view;
}
