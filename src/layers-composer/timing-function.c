#define _POSIX_C_SOURCE 200809L

#include "sc-layer-shell.h"

void
timing_function_create()
{
	timing_function = calloc(1, sizeof(struct sc_timing_function_v1));
	// easeout
	timing_function->cx0 = 0.0;
	timing_function->cy0 = 0.0;
	timing_function->cx1 = 0.0;
	timing_function->cy1 = 0.0;
	timing_function->cx2 = 0.58;
	timing_function->cy2 = 1.0;
	timing_function->cx3 = 1.0;
	timing_function->cy3 = 1.0;
}

void timing_function_handle_set_name(struct wl_client *client,
			struct wl_resource *resource,
			const char *name)
{

}

void timing_function_handle_set_control_points(struct wl_client *client,
				struct wl_resource *resource,
				wl_fixed_t c1x,
				wl_fixed_t c1y,
				wl_fixed_t c2x,
				wl_fixed_t c2y)
{

}

void timing_function_handle_destroy(struct wl_client *client,
		struct wl_resource *resource)
{

}


static const struct sc_timing_function_v1_interface
	sc_timing_implementation = {
		.set_name = timing_function_handle_set_name,
		.set_control_points = timing_function_handle_set_control_points,
		.destroy = timing_function_handle_destroy,
};

