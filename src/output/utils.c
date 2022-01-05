#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <math.h>
#include <wlr/util/box.h>

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


void sc_box_from_layout_to_output(struct sc_output *output, struct
		wlr_box *box)
{
	struct wlr_box *output_box = output->output_box;

	box->x = box->x - output_box->x;
	box->y = box->y - output_box->y;

	sc_output_scale_box(output, box);
}

bool
sc_output_intersect_view(struct sc_output *output, struct sc_view *view)
{
	struct wlr_box intersection;
	return wlr_box_intersection(&intersection, output->output_box, &view->frame);
}

