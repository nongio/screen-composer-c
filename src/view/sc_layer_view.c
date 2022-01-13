#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>

#include "log.h"
#include "sc_compositor_workspace.h"
#include "sc_layer_view.h"
#include "sc_toplevel_view.h"
#include "sc_view.h"

static void
layer_map(struct wl_listener *listener, void *data)
{

	DLOG("layer_map\n");

	struct sc_layer_view *layer_view =
		wl_container_of(listener, layer_view, on_map);

	struct sc_view *view = (struct sc_view *) layer_view;
	sc_view_map(view);

	sc_compositor_add_layer(view->compositor, layer_view);
}

static void
layer_unmap(struct wl_listener *listener, void *data)
{
	DLOG("layer_unmap\n");
	struct sc_layer_view *layer_view =
		wl_container_of(listener, layer_view, on_unmap);
	struct sc_view *view = (struct sc_view *) layer_view;
	view->mapped = false;

	wl_list_remove(&layer_view->link);
}

static void
layer_destroy(struct wl_listener *listener, void *data)
{
	DLOG("layer_destroy\n");
	struct sc_layer_view *layer_view =
		wl_container_of(listener, layer_view, on_destroy);

	wl_list_remove(&layer_view->on_map.link);
	wl_list_remove(&layer_view->on_unmap.link);
	wl_list_remove(&layer_view->on_destroy.link);

	free(layer_view);
}

static void
layer_surface_commit(struct wl_listener *listener, void *data)
{
	DLOG("layer_surface_commit\n");
	struct sc_layer_view *layer_view =
		wl_container_of(listener, layer_view, on_surface_commit);

	struct sc_view *view = (struct sc_view *) layer_view;

	struct wlr_layer_surface_v1 *layer_surface = layer_view->layer_surface;
	if (view->mapped != layer_surface->mapped ||
		(layer_surface->added && !layer_surface->configured)) {

		wlr_layer_surface_v1_configure(layer_surface, view->frame.width,
									   view->frame.height);
	}
	sc_output_add_damage_from_view(view->output, view, true);
}

void
layer_for_each_surface(struct sc_view *view,
						  wlr_surface_iterator_func_t iterator, void *user_data)
{
}

void
layer_for_each_popup_surface(struct sc_view *view,
								wlr_surface_iterator_func_t iterator,
								void *user_data)
{
}

static struct sc_view_impl layer_view_impl = {
	.for_each_surface = layer_for_each_surface,
	.for_each_popup_surface = layer_for_each_popup_surface,
};


struct sc_layer_view *
sc_layer_view_create(struct wlr_layer_surface_v1 *layer_surface,
					 struct sc_compositor *compositor)
{
	DLOG("sc_layer_view_create\n");
	struct sc_layer_view *layer_view = calloc(1, sizeof(struct sc_layer_view));
	struct sc_view *view = (struct sc_view *) layer_view;

	view->frame = (struct wlr_box){
		.x = layer_surface->pending.margin.left,
		.y = layer_surface->pending.margin.top,
		.width = layer_surface->pending.desired_width,
		.height = layer_surface->pending.desired_height,
	};

	view->compositor = compositor;
	sc_view_init(view, SC_VIEW_LAYER, &layer_view_impl, layer_surface->surface);

	layer_view->layer_surface = layer_surface;

	layer_view->on_map.notify = layer_map;
	wl_signal_add(&layer_surface->events.map, &layer_view->on_map);

	layer_view->on_unmap.notify = layer_unmap;
	wl_signal_add(&layer_surface->events.unmap, &layer_view->on_unmap);

	layer_view->on_destroy.notify = layer_destroy;
	wl_signal_add(&layer_surface->events.destroy, &layer_view->on_destroy);

	layer_view->on_surface_commit.notify = layer_surface_commit;
	wl_signal_add(&layer_surface->surface->events.commit,
				  &layer_view->on_surface_commit);

	return layer_view;
}
