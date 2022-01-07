#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>

#include "log.h"
#include "sc_output.h"
#include "sc_view.h"

int
main(int argc, char **argv)
{
	struct sc_output output;
	output.wlr_output = malloc(sizeof(struct wlr_output));
	output.output_box = &(struct wlr_box) {
		.x = 0,
		.y = 0,
		.width = 1024,
		.height = 768,
	};
	struct sc_view view;
	view.frame = (struct wlr_box) {
		.x = 0,
		.y = 0,
		.width = 100,
		.height = 100,
	};
	output.wlr_output->scale = 1;

	assert(sc_output_intersect_view(&output, &view) == true);

	view.frame = (struct wlr_box) {
		.x = 2000,
		.y = 0,
		.width = 100,
		.height = 100,
	};

	assert(sc_output_intersect_view(&output, &view) == false);

	return 0;
}

