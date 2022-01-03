#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/util/region.h>

#include "log.h"
#include "sc_output.h"
#include "sc_view.h"

static int scale_length(int length, int offset, float scale) {
	return round((offset + length) * scale) - round(offset * scale);
}

void sc_output_scale_box(struct sc_output *output, struct wlr_box *box) {
	float scale = output->wlr_output->scale;

	box->width = scale_length(box->width, box->x, scale);
	box->height = scale_length(box->height, box->y, scale);
	box->x = round(box->x * scale);
	box->y = round(box->y * scale);
}

struct damage_surface_iterator_data {
	struct sc_output *output;
	bool whole;
};

static void damage_surface_iterator(struct wlr_surface *surface, int x, int y,
		void * data) {

	struct damage_surface_iterator_data *dsi = data;
	bool whole = dsi->whole;

	struct sc_output *output = dsi->output;

	struct wlr_box *output_box =
		wlr_output_layout_get_box(output->layout, output->wlr_output);

	struct wlr_box box = {
		.x = surface->sx - output_box->x,
		.y = surface->sy - output_box->y,
		.width = surface->current.width,
		.height = surface->current.height,
	};


	sc_output_scale_box(output, &box);

	LOG("damage_surface_iterator %d,%d %dx%d\n", box.x, box.y, box.width, box.height);

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
	pixman_region32_translate(&damage, box.x, box.y);
	wlr_output_damage_add(output->damage, &damage);
	pixman_region32_fini(&damage);

	if (whole) {
		wlr_output_damage_add_box(output->damage, &box);
	}

	//if (!wl_list_empty(&surface->current.frame_callback_list)) {
	//	wlr_output_schedule_frame(output->wlr_output);
	//}
}

void
sc_output_damage_view(struct sc_output *output, struct sc_view *view, bool whole)
{

	LOG("sc_output_damage_view \n");
	//if (!view_is_visible(view)) {
	//	return;
	//}
	struct damage_surface_iterator_data data = {
		.output = output,
		.whole = false,
	};

	sc_view_for_each_surface(view, damage_surface_iterator, &data);
}
