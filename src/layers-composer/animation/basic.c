#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-server-core.h>

#include "log.h"
#include "sc-layer-shell-animation.h"
#include "sc-layer-shell.h"
#include "sc-layer-unstable-v1-protocol.h"
#include "utils.h"
static const struct sc_basic_animation_v1_interface
	sc_basic_animation_implementation;
struct sc_basic_animation_v1 *
basic_animation_from_resource(struct wl_resource *resource)
{
	assert(wl_resource_instance_of(resource, &sc_basic_animation_v1_interface,
								   &sc_basic_animation_implementation));
	return wl_resource_get_user_data(resource);
}

static void
basic_animation_destroy(struct sc_basic_animation_v1 *animation)
{

	// wlr_signal_emit_safe(&surface->events.destroy, surface);
	wl_resource_set_user_data(animation->resource, NULL);

	free(animation);
}

static void
basic_animation_resource_destroy(struct wl_resource *resource)
{
	struct sc_basic_animation_v1 *animation =
		basic_animation_from_resource(resource);
	if (animation != NULL) {
		basic_animation_destroy(animation);
	}
}

static void *
basic_animation_value_at(double fraction, void *data)
{

	struct sc_basic_animation_v1 *basic = data;
	double ease = bezier_easing_function(
		basic->timing_function->cx1, basic->timing_function->cy1,
		basic->timing_function->cx2, basic->timing_function->cy2, fraction);

	void *value;
	if (basic->type == SC_ANIMATION_V1_ANIMATION_VALUE_TYPE_VALUE) {
		// FIXME leak
		double *double_value = calloc(1, sizeof(double));
		*double_value = interpolate(*(double *) basic->from_value,
									*(double *) basic->to_value, ease);
		LOG("animation value: %f\n", *double_value);
		value = double_value;
	}

	return value;
}

static const struct sc_animation_impl basic_value_provider = {
	.value = basic_animation_value_at,
};

/**
 * an animation value provider, it interpolates between 2 or 3 values
 *
 * The basic animation can interpolate 2 or 3 values using the
 * provided timing function.
 * @param value_type type of values this animation will interpolate
 */
void
shell_handle_get_basic_animation(struct wl_client *wl_client,
								 struct wl_resource *client_resource,
								 uint32_t id, uint32_t value_type,
								 struct wl_resource *animation_resource,
								 struct wl_resource *timing_resource)
{
	struct sc_layer_shell_v1 *shell =
		layer_shell_from_resource(client_resource);

	struct sc_animation_v1 *animation =
		animation_from_resource(animation_resource);
	struct sc_timing_function_v1 *timing =
		timing_function_from_resource(timing_resource);

	struct sc_basic_animation_v1 *basic_animation =
		calloc(1, sizeof(struct sc_basic_animation_v1));

	if (basic_animation == NULL) {
		wl_client_post_no_memory(wl_client);
		return;
	}

	basic_animation->shell = shell;
	basic_animation->timing_function = timing;
	basic_animation->animation = animation;
	basic_animation->type = value_type;

	// FIXME there should be a different error in the protocol
	if (!animation_set_value_provider(animation, &basic_value_provider,
									  basic_animation, client_resource,
									  SC_SHELL_UNSTABLE_V1_ERROR_ROLE)) {
		free(basic_animation);
		return;
	}

	basic_animation->resource =
		wl_resource_create(wl_client, &sc_basic_animation_v1_interface,
						   wl_resource_get_version(client_resource), id);

	if (basic_animation->resource == NULL) {
		free(basic_animation);
		wl_client_post_no_memory(wl_client);
		return;
	}

	wl_signal_init(&basic_animation->events.destroy);

	wl_resource_set_implementation(
		basic_animation->resource, &sc_basic_animation_implementation,
		basic_animation, basic_animation_resource_destroy);
}

void
basic_handle_set_from_value(struct wl_client *client,
							struct wl_resource *resource,
							struct wl_array *from_value)
{
	struct sc_basic_animation_v1 *animation =
		basic_animation_from_resource(resource);

	if (!animation) {
		return;
	}
	animation->from_value =
		decode_animation_value_for_type(from_value, animation->type);
}

void
basic_handle_set_to_value(struct wl_client *client,
						  struct wl_resource *resource,
						  struct wl_array *to_value)
{
	struct sc_basic_animation_v1 *animation =
		basic_animation_from_resource(resource);

	if (!animation) {
		return;
	}
	animation->to_value =
		decode_animation_value_for_type(to_value, animation->type);
}

void
basic_handle_set_by_value(struct wl_client *client,
						  struct wl_resource *resource,
						  struct wl_array *by_value)
{
	struct sc_basic_animation_v1 *animation =
		basic_animation_from_resource(resource);

	if (!animation) {
		return;
	}
	animation->by_value =
		decode_animation_value_for_type(by_value, animation->type);
}

void
basic_handle_set_timing_function(struct wl_client *client,
								 struct wl_resource *resource,
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
