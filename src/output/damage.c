#define _POSIX_C_SOURCE 200809L
#include <wlr/util/region.h>

#include "log.h"
#include "sc_output.h"
#include "sc_view.h"

struct damage_surface_iterator_data {
	struct sc_output *output;
	bool whole;
};

static void add_damage_surface_iterator(struct wlr_surface *surface, int x, int y,
		void * data) {

	struct damage_surface_iterator_data *dsi = data;
	bool whole = dsi->whole;

	struct sc_output *output = dsi->output;

	struct wlr_box surface_box = {
		.x = surface->sx,
		.y = surface->sy,
		.width = surface->current.width,
		.height = surface->current.height,
	};

	sc_box_from_layout_to_output(output, &surface_box);

	DLOG("damage_surface_iterator %d,%d %dx%d\n", surface_box.x, surface_box.y, surface_box.width, surface_box.height);

	pixman_region32_t damage;
	pixman_region32_init(&damage);
	wlr_surface_get_effective_damage(surface, &damage);
	wlr_region_scale(&damage, &damage, output->wlr_output->scale);
	if (ceil(output->wlr_output->scale) > surface->current.scale) {
		// When scaling up a surface, it'll become blurry so we need to
		// expand the damage region
		wlr_region_expand(&damage, &damage,
			ceil(output->wlr_output->scale) - surface->current.scale);
	}
	pixman_region32_translate(&damage, surface_box.x, surface_box.y);
	wlr_output_damage_add(output->damage, &damage);
	pixman_region32_fini(&damage);

	if (whole) {
		wlr_output_damage_add_box(output->damage, &surface_box);
	}

	//if (!wl_list_empty(&surface->current.frame_callback_list)) {
	//	wlr_output_schedule_frame(output->wlr_output);
	//}
}

void
sc_output_add_damage_from_view(struct sc_output *output, struct sc_view *view, bool whole)
{

	DLOG("sc_output_add_damage_from_view \n");
	if (!sc_view_is_visible(view)) {
		return;
	}
	struct damage_surface_iterator_data data = {
		.output = output,
		.whole = whole,
	};

	sc_view_for_each_surface(view, add_damage_surface_iterator, &data);
}

