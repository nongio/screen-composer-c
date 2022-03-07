#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-server-core.h>

#include "log.h"
#include "sc-layer-shell-animation.h"
#include "sc-layer-shell-layer.h"
#include "sc-layer-shell.h"
#include "sc-layer-unstable-v1-protocol.h"

static void
animation_handle_set_duration(struct wl_client *client,
							  struct wl_resource *resource, wl_fixed_t duration)
{
}

static void
animation_handle_set_keypath(struct wl_client *client,
							 struct wl_resource *resource, const char *keypath)
{
}

static void
animation_handle_set_cumulative(struct wl_client *client,
								struct wl_resource *resource,
								uint32_t cumulative)
{
}

static void
animation_handle_set_additive(struct wl_client *client,
							  struct wl_resource *resource, uint32_t additive)
{
}
static void
animation_handle_set_removed_on_completion(struct wl_client *client,
										   struct wl_resource *resource,
										   uint32_t removed_on_completion)
{
}

const struct sc_animation_v1_interface sc_animation_implementation = {
	.set_duration = animation_handle_set_duration,
	.set_keypath = animation_handle_set_keypath,
	.set_cumulative = animation_handle_set_cumulative,
	.set_additive = animation_handle_set_additive,
	.set_removed_on_completion = animation_handle_set_removed_on_completion,
};
