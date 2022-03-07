#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-server-core.h>

#include "sc-layer-unstable-v1-protocol.h"

void
set_value_for_key(struct wl_client *client, struct wl_resource *resource,
				  wl_fixed_t value, const char *key)
{
}

static const struct sc_filter_v1_interface sc_filter_implementation = {
	.set_value_for_key = filter_handle_set_value_for_key,
};
