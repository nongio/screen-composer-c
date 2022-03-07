#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-server-core.h>

#include "sc-layer-unstable-v1-protocol.h"

void
basic_handle_set_from_value(struct wl_client *client, struct wl_resource *resource,
			   struct wl_array *from_value)
{
}

void
basic_handle_set_to_value(struct wl_client *client, struct wl_resource *resource,
			 struct wl_array *to_value)
{
}

void
basic_handle_set_by_value(struct wl_client *client, struct wl_resource *resource,
			 struct wl_array *by_value)
{
}

void
basic_handle_set_timing_function(struct wl_client *client, struct wl_resource *resource,
					struct wl_resource *timing_function)
{
}

static const struct sc_basic_animation_v1_interface
	sc_basic_animation_implementation = {
		.set_from_value = basic_handle_set_from_value,
		.set_to_value = basic_handle_set_to_value,
		.set_by_value = basic_handle_set_by_value,
		.set_timing_function = basic_handle_set_timing_function,
};
