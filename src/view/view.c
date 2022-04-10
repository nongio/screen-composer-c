#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/types/wlr_surface.h>

#include "log.h"
#include "sc_compositor.h"
#include "sc_geometry.h"
#include "sc_output.h"
#include "sc_view.h"

static void
view_surface_commit_handler(struct wl_listener *listener, void *data)
{
	struct sc_view *view = wl_container_of(listener, view, on_surface_commit);
	// subviews informations can be committed together with the parent
	struct sc_view *subview;
	wl_list_for_each (subview, &view->children, link) {
		if (subview->subsurface) {
			subview->frame.x = subview->subsurface->current.x;
			subview->frame.y = subview->subsurface->current.y;
			subview->frame.width = subview->surface->current.width;
			subview->frame.height = subview->surface->current.height;
		}
	}

	if (view->impl->commit) {
		view->impl->commit(view);
		return;
	}

	if (view->subsurface) {
		view->frame.x = view->subsurface->current.x;
		view->frame.y = view->subsurface->current.y;
	} else {
		view->frame.x = 0;
		view->frame.y = 0;
	}
	view->frame.width = view->surface->current.width;
	view->frame.height = view->surface->current.height;

	if (view->parent != NULL) {
		sc_output_add_damage_from_view(view->parent->output, view->parent,
									   false);
	} else {
		sc_output_add_damage_from_view(view->output, view, false);
	}
}

static void
subview_destroy_handler(struct wl_listener *listener, void *data)
{
	struct sc_view *subview =
		wl_container_of(listener, subview, on_subview_destroy);

	if (subview->output == NULL) {
		return;
	}

	if (subview->parent != NULL) {
		sc_output_add_damage_from_view(subview->parent->output, subview->parent,
									   false);
	}

	sc_view_destroy(subview);
	free(subview);
}

static void
for_each_surface(struct sc_view *view, wlr_surface_iterator_func_t iterator,
				 void *user_data)
{
	wlr_surface_for_each_surface(view->surface, iterator, user_data);
}

static void
for_each_popup_surface(struct sc_view *view,
					   wlr_surface_iterator_func_t iterator, void *user_data)
{

	// wlr_xdg_surface_for_each_popup_surface(view->wlr_xdg_surface, iterator,
	// user_data);
}

static struct wlr_surface *
surface_at(struct sc_view *view, double x, double y, double *sx, double *sy)
{
	return NULL;
}

static struct sc_view_impl view_impl = {
	.for_each_surface = for_each_surface,
	.for_each_popup_surface = for_each_popup_surface,
	.surface_at = surface_at,
};

static struct sc_view *
view_subsurface_create(struct wlr_subsurface *subsurface, struct sc_view *view)
{
	struct sc_view *subview = calloc(1, sizeof(struct sc_view));
	sc_view_init(subview, SC_VIEW_SUBVIEW, NULL, subsurface->surface);
	subview->subsurface = subsurface;
	subview->output = view->output;
	subview->parent = view;

	// subsurfaces are mapped by default
	subview->mapped = true;
	subview->on_subview_destroy.notify = subview_destroy_handler;
	wl_signal_add(&subsurface->events.destroy, &subview->on_subview_destroy);

	return subview;
}

static void
view_subsurface_new(struct wl_listener *listener, void *data)
{
	struct sc_view *view = wl_container_of(listener, view, on_subsurface_new);

	struct wlr_subsurface *subsurface = data;

	struct sc_view *subview = view_subsurface_create(subsurface, view);

	wl_list_insert(&view->children, &subview->link);
}

void
sc_view_init(struct sc_view *view, enum sc_view_type type,
			 struct sc_view_impl *impl, struct wlr_surface *surface)
{
	view->type = type;
	if (impl != NULL) {
		view->impl = impl;
	} else {
		view->impl = &view_impl;
	}

	view->surface = surface;
	view->texture_attributes = malloc(sizeof(struct sc_texture_attributes));
	wl_list_init(&view->children);

	view->on_surface_commit.notify = view_surface_commit_handler;
	wl_signal_add(&view->surface->events.commit, &view->on_surface_commit);

	view->on_subsurface_new.notify = view_subsurface_new;
	wl_signal_add(&view->surface->events.new_subsurface,
				  &view->on_subsurface_new);
}

void
sc_view_destroy(struct sc_view *view)
{
	struct sc_view *subview;
	wl_list_for_each (subview, &view->children, link) {
		subview->parent = NULL;
	}
	wl_list_remove(&view->link);
	wl_list_remove(&view->on_subsurface_new.link);
	wl_list_remove(&view->on_surface_commit.link);
	wl_list_remove(&view->on_subview_destroy.link);
	free(view->texture_attributes);
}

void
sc_view_damage_part(struct sc_view *view)
{
	if (view->output != NULL) {
		sc_output_add_damage_from_view(view->output, view, false);
	}
}

void
sc_view_damage_whole(struct sc_view *view)
{
	if (view->output != NULL) {
		sc_output_add_damage_from_view(view->output, view, true);
	}
}

void
sc_view_get_absolute_position(struct sc_view *view, struct sc_point *p)
{
	p->x += view->frame.x;
	p->y += view->frame.y;

	if (view->parent) {
		sc_view_get_absolute_position(view->parent, p);
	}
}

void
sc_view_map(struct sc_view *view)
{
	view->mapped = true;
	struct sc_point p = {.x = 0, .y = 0};
	sc_view_get_absolute_position(view, &p);

	if (!view->output) {
		struct sc_output *output = sc_compositor_output_at(p.x, p.y);
		view->output = output;
	}


	struct sc_view *subview;
	wl_list_for_each (subview, &view->children, link) {
		subview->output = view->output;
		subview->mapped = true;
	}
	
	sc_view_damage_whole(view);
}

void
sc_view_unmap(struct sc_view *view)
{
	sc_view_damage_whole(view);
	view->mapped = false;
}
void
sc_view_for_each_surface(struct sc_view *view,
						 wlr_surface_iterator_func_t iterator, void *user_data)
{
	if (!view->surface) {
		return;
	}
	if (view->impl->for_each_surface) {
		view->impl->for_each_surface(view, iterator, user_data);
	} else {
		wlr_surface_for_each_surface(view->surface, iterator, user_data);
	}
}

void
sc_view_for_each_popup_surface(struct sc_view *view,
							   wlr_surface_iterator_func_t iterator,
							   void *user_data)
{
	if (!view->surface) {
		return;
	}
	if (view->impl->for_each_popup_surface) {
		view->impl->for_each_popup_surface(view, iterator, user_data);
	}
}

bool
sc_view_is_visible(struct sc_view *view)
{
	return view->mapped;
}

struct wlr_surface *
sc_view_surface_at(struct sc_view *view, double x, double y, double *sx,
				   double *sy)
{
	if (!view->mapped) {
		return NULL;
	}
	if (view->impl->surface_at) {
		return view->impl->surface_at(view, x, y, sx, sy);
	}

	return NULL;
}

void
sc_view_set_output(struct sc_view *view, struct sc_output *output)
{
	view->output = output;
}

void
sc_view_activate(struct sc_view *view)
{
	if (!view->mapped) {
		return;
	}
	if (view->impl->activate) {
		view->impl->activate(view);
	}
	sc_view_damage_whole(view);
}

void
sc_view_deactivate(struct sc_view *view)
{
	if (!view->mapped) {
		return;
	}
	if (view->impl->deactivate) {
		view->impl->deactivate(view);
	}
	sc_view_damage_whole(view);
}
