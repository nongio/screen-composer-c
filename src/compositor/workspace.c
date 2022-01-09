#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_compositor_workspace.h"
#include "sc_view.h"
#include "sc_layer_view.h"
#include "sc_toplevel_view.h"
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
						   struct sc_toplevel_view *toplevelview)
{
	wl_list_insert(&compositor->current_workspace->views_toplevel, &toplevelview->link);
}

void
sc_compositor_add_layer(struct sc_compositor *compositor,
						struct sc_layer_view *layer)
{
	switch (layer->layer_surface->pending.layer) {
	case ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND:
		DLOG("add ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND\n");
		wl_list_insert(&compositor->current_workspace->layers_background,
					   &layer->link);
		break;
	case ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM:
		DLOG("add ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM\n");
		wl_list_insert(&compositor->current_workspace->layers_bottom,
					   &layer->link);

		break;
	case ZWLR_LAYER_SHELL_V1_LAYER_TOP:
		DLOG("add ZWLR_LAYER_SHELL_V1_LAYER_TOP\n");
		wl_list_insert(&compositor->current_workspace->layers_top,
					   &layer->link);

		break;
	case ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY:
		DLOG("add ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY\n");
		wl_list_insert(&compositor->current_workspace->layers_overlay,
					   &layer->link);
		break;
	}
}

struct sc_view *sc_composer_view_at(struct sc_compositor *compositor, double x, double
		y, struct wlr_surface **surface, double *sx, double *sy)
{
	struct sc_toplevel_view *toplevel_view;
	struct sc_workspace *workspace = compositor->current_workspace;
	wl_list_for_each (toplevel_view, &workspace->views_toplevel, link) {
		struct sc_view *view = &toplevel_view->super;
		*surface = sc_view_surface_at(view, x, y, sx, sy);

		if (*surface != NULL) {
			return (struct sc_view *)view;
		}
	}
	return NULL;
}

void
sc_composer_focus_view(struct sc_compositor *compositor, struct sc_view *view)
{
	DLOG("focus view\n");

	if (view == NULL) {
		DLOG("view == NULL\n");
		return;
	}
	if (view == compositor->current_view) {
		DLOG("view == compositor->current_view\n");
		return;
	}
	struct wlr_seat *seat = compositor->seat;
	struct wlr_surface *prev_surface = seat->keyboard_state.focused_surface;

	if (prev_surface == view->surface) {
		// Don't re-focus an already focused surface.
		DLOG("prev_surface == view->surface\n");
		return;
	}
	if (compositor->current_view != NULL) {
		sc_view_deactivate(compositor->current_view);
	}
	struct wlr_keyboard *keyboard = wlr_seat_get_keyboard(seat);

	compositor->current_view = view;

	// FIXME if the view is a layer don't add it to the views array!!
	//
	//  if ([view isKindOfClass:[SCTopLevelView class]])
	//    {
	//      SCTopLevelView * topview = (SCTopLevelView *)view;
	//
	//      [self->currentDesktop->topLevelViews removeObject:topview];
	//      int count = [self->currentDesktop->topLevelViews count];
	//      [self->currentDesktop->topLevelViews insertObject:topview
	//      atIndex:(count)];
	//
	//      /* Activate the new surface */
	//      [topview activate];
	//    }
	sc_view_activate(compositor->current_view);

	/*
	 * Tell the seat to have the keyboard enter this surface. wlroots will keep
	 * track of this and automatically send key events to the appropriate
	 * clients without additional work on your part.
	 */

	wlr_seat_keyboard_notify_enter(seat, view->surface, keyboard->keycodes,
								   keyboard->num_keycodes,
								   &keyboard->modifiers);
	sc_view_damage_whole(compositor->current_view);
}
