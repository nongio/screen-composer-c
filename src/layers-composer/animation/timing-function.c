#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>

#include "sc-layer-shell.h"

static const struct sc_timing_function_v1_interface sc_timing_function_implementation;


struct sc_timing_function_v1 *
timing_function_from_resource(struct wl_resource *resource)
{
	assert(wl_resource_instance_of(resource, &sc_timing_function_v1_interface,
								   &sc_timing_function_implementation));
	return wl_resource_get_user_data(resource);
}

static void
timing_function_destroy(struct sc_timing_function_v1 *timing)
{

	// wlr_signal_emit_safe(&surface->events.destroy, surface);
	wl_resource_set_user_data(timing->resource, NULL);

	free(timing);
}

static void
timing_function_resource_destroy(struct wl_resource *resource)
{
	struct sc_timing_function_v1 *timing = timing_function_from_resource(resource);
	if (timing != NULL) {
		timing_function_destroy(timing);
	}
}

/**
	* a timing function described by a bezier curve
	* by default it returns an ease out timing function
	*
	*/
void shell_handle_get_timing_function(struct wl_client *wl_client,
				struct wl_resource *client_resource,
				uint32_t id)
{
	struct sc_layer_shell_v1 *shell =
		layer_shell_from_resource(client_resource);

	struct sc_timing_function_v1 *timing =
		calloc(1, sizeof(struct sc_timing_function_v1));

	// easeout
	timing->cx1 = 0.0;
	timing->cy1 = 0.0;
	timing->cx2 = 0.58;
	timing->cy2 = 1.0;

	if (timing == NULL) {
		wl_client_post_no_memory(wl_client);
		return;
	}

	timing->shell = shell;

	timing->resource =
		wl_resource_create(wl_client, &sc_timing_function_v1_interface,
						   wl_resource_get_version(client_resource), id);

	if (timing->resource == NULL) {
		free(timing);
		wl_client_post_no_memory(wl_client);
		return;
	}

	wl_signal_init(&timing->events.destroy);

	wl_resource_set_implementation(timing->resource,
								   &sc_timing_function_implementation, timing,
								   timing_function_resource_destroy);
}

void
timing_function_handle_set_control_points(struct wl_client *client,
										  struct wl_resource *resource,
										  wl_fixed_t c1x, wl_fixed_t c1y,
										  wl_fixed_t c2x, wl_fixed_t c2y)
{
}

void
timing_function_handle_destroy(struct wl_client *client,
							   struct wl_resource *resource)
{
}

static const struct sc_timing_function_v1_interface sc_timing_function_implementation = {
	.set_control_points = timing_function_handle_set_control_points,
	.destroy = timing_function_handle_destroy,
};
