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

const struct sc_animation_v1_interface sc_animation_implementation;

struct sc_animation_v1 *
animation_from_resource(struct wl_resource *resource)
{
	assert(wl_resource_instance_of(resource, &sc_animation_v1_interface,
								   &sc_animation_implementation));
	return wl_resource_get_user_data(resource);
}

static void
animation_destroy(struct sc_animation_v1 *animation)
{

	// wlr_signal_emit_safe(&surface->events.destroy, surface);
	wl_resource_set_user_data(animation->resource, NULL);

	free(animation);
}

static void
animation_resource_destroy(struct wl_resource *resource)
{
	struct sc_animation_v1 *animation = animation_from_resource(resource);
	if (animation != NULL) {
		animation_destroy(animation);
	}
}
/**
 * creates an animation object that can run on the compositor
 *
 * An animation interface, defines the basic properties of an
 * animation: duration, speed and how the animation is applied. The
 * be used, it needs to have a value provider (eg. basic animation)
 * and to be attached to an object. The compositor will then take
 * care of running the animation and apply the calculated value to
 * the object.
 * @param duration duration of the animation
 * @param speed speed of the animation
 * @param repeat_count how many times the animation should run
 * @param autoreverse
 */
void
shell_handle_get_animation(struct wl_client *wl_client,
						   struct wl_resource *client_resource, uint32_t id,
						   wl_fixed_t duration, wl_fixed_t speed,
						   int32_t repeat_count, uint32_t autoreverse)
{
	struct sc_layer_shell_v1 *shell =
		layer_shell_from_resource(client_resource);

	struct sc_animation_v1 *animation =
		calloc(1, sizeof(struct sc_animation_v1));

	if (animation == NULL) {
		wl_client_post_no_memory(wl_client);
		return;
	}

	animation->shell = shell;

	animation->resource =
		wl_resource_create(wl_client, &sc_animation_v1_interface,
						   wl_resource_get_version(client_resource), id);
	if (animation->resource == NULL) {
		free(animation);
		wl_client_post_no_memory(wl_client);
		return;
	}

	wl_signal_init(&animation->events.destroy);

	wl_resource_set_implementation(animation->resource,
								   &sc_animation_implementation, animation,
								   animation_resource_destroy);

	animation->duration = wl_fixed_to_double(duration);
	animation->speed = wl_fixed_to_double(speed);
	animation->repeat_count = repeat_count;
	animation->autoreverse = autoreverse;
}

bool animation_set_value_provider(struct sc_animation_v1 *animation,
		const struct sc_animation_impl *value_provider, void *impl_data,
		struct wl_resource *error_resource, uint32_t error_code) {
	assert(value_provider != NULL);

	if (animation->impl != NULL && animation->impl != value_provider) {
		if (error_resource != NULL) {
			wl_resource_post_error(error_resource, error_code,
				"Cannot assign value provider %s to animation@%" PRIu32 ", already has a provider %s\n",
				value_provider->name, wl_resource_get_id(animation->resource),
				animation->impl->name);
		}
		return false;
	}
	if (animation->impl_data != NULL && animation->impl_data != impl_data) {
		wl_resource_post_error(error_resource, error_code,
			"Cannot reassign provider %s to animation@%" PRIu32 ","
			"role object still exists", value_provider->name,
			wl_resource_get_id(animation->resource));
		return false;
	}

	animation->impl = value_provider;
	animation->impl_data = impl_data;
	return true;
}

static void
animation_handle_set_duration(struct wl_client *client,
							  struct wl_resource *resource, wl_fixed_t duration)
{
	struct sc_animation_v1 *animation = animation_from_resource(resource);

	if (!animation) {
		return;
	}
	animation->duration = wl_fixed_to_double(duration);
}

static void
animation_handle_set_keypath(struct wl_client *client,
							 struct wl_resource *resource, const char *keypath)
{
	struct sc_animation_v1 *animation = animation_from_resource(resource);

	if (!animation) {
		return;
	}
	animation->keypath = strdup(keypath);
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

void * animation_value_at(struct sc_animation_v1 *animation, double timestamp) {
	double fraction = (timestamp - animation->begin_time) / animation->duration;
	if(fraction >= 1.0) {
		animation->is_running = false;
		fraction = 1.0;
	}
	return animation->impl->value(fraction, animation->impl_data);
}

const struct sc_animation_v1_interface sc_animation_implementation = {
	.set_duration = animation_handle_set_duration,
	.set_keypath = animation_handle_set_keypath,
	.set_cumulative = animation_handle_set_cumulative,
	.set_additive = animation_handle_set_additive,
	.set_removed_on_completion = animation_handle_set_removed_on_completion,
};
