#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>

#include "log.h"
#include "sc_output.h"

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
	struct wlr_box test_box = {
		.x = 10,
		.y = 10,
		.width = 100,
		.height = 100,
	};

	// check 0,0 scale 1
	output.wlr_output->scale = 1;
	sc_box_from_layout_to_output(&output, &test_box);
	assert(test_box.x == 10);
	assert(test_box.y == 10);
	assert(test_box.width == 100);
	assert(test_box.height == 100);
	LOG("%d,%d %dx%d\n", test_box.x, test_box.y, test_box.width, test_box.height);
	// check 0,0 scale 2
	output.wlr_output->scale = 2;
	sc_box_from_layout_to_output(&output, &test_box);
	assert(test_box.x == 20);
	assert(test_box.y == 20);
	assert(test_box.width == 200);
	assert(test_box.height == 200);
	LOG("%d,%d %dx%d\n", test_box.x, test_box.y, test_box.width, test_box.height);

	// check 1024,0 scale 1
	output.wlr_output->scale = 1;
	test_box = (struct wlr_box){
		.x = 10,
		.y = 10,
		.width = 100,
		.height = 100,
	};
	output.output_box = &(struct wlr_box) {
		.x = 1024,
		.y = 0,
		.width = 1024,
		.height = 768,
	};
	sc_box_from_layout_to_output(&output, &test_box);
	assert(test_box.x == -1014);
	assert(test_box.y == 10);
	assert(test_box.width == 100);
	assert(test_box.height == 100);

	LOG("%d,%d %dx%d\n", test_box.x, test_box.y, test_box.width, test_box.height);

	// check 1024,768 scale 1
	output.wlr_output->scale = 1;
	test_box = (struct wlr_box){
		.x = 10,
		.y = 10,
		.width = 100,
		.height = 100,
	};
	output.output_box = &(struct wlr_box) {
		.x = 1024,
		.y = 0,
		.width = 1024,
		.height = 768,
	};
	sc_box_from_layout_to_output(&output, &test_box);
	assert(test_box.x == -1014);
	assert(test_box.y == 10);
	assert(test_box.width == 100);
	assert(test_box.height == 100);

	LOG("%d,%d %dx%d\n", test_box.x, test_box.y, test_box.width, test_box.height);

	// check 1024,768 scale 2
	output.wlr_output->scale = 2;
	test_box = (struct wlr_box){
		.x = 10,
		.y = 10,
		.width = 100,
		.height = 100,
	};
	output.output_box = &(struct wlr_box) {
		.x = 1024,
		.y = 0,
		.width = 1024,
		.height = 768,
	};
	sc_box_from_layout_to_output(&output, &test_box);
	assert(test_box.x == -2028);
	assert(test_box.y == 20);
	assert(test_box.width == 200);
	assert(test_box.height == 200);
	LOG("%d,%d %dx%d\n", test_box.x, test_box.y, test_box.width, test_box.height);


	// check 1024,768 scale 2
	output.wlr_output->scale = 2;
	test_box = (struct wlr_box){
		.x = 10,
		.y = 10,
		.width = 100,
		.height = 100,
	};
	output.output_box = &(struct wlr_box) {
		.x = 1024,
		.y = 768,
		.width = 1024,
		.height = 768,
	};
	sc_box_from_layout_to_output(&output, &test_box);
	LOG("%d,%d %dx%d\n", test_box.x, test_box.y, test_box.width, test_box.height);
	assert(test_box.x == -2028);
	assert(test_box.y == -1516);
	assert(test_box.width == 200);
	assert(test_box.height == 200);


	return 0;
}

