#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_surface.h>

#include "log.h"
#include "sc-layer-shell-animation.h"
#include "sc-layer-shell-layer.h"
#include "sc-layer-shell.h"
#include "sc-layer-unstable-v1-protocol.h"
#include "sc_output.h"
#include "utils.h"

static const struct sc_layer_surface_v1_interface
	sc_layer_surface_implementation;

static struct sc_layer_surface_v1 *
layer_surface_from_resource(struct wl_resource *resource)
{
	assert(wl_resource_instance_of(resource, &sc_layer_surface_v1_interface,
								   &sc_layer_surface_implementation));
	return wl_resource_get_user_data(resource);
}

static void
layer_surface_unmap(struct sc_layer_surface_v1 *surface)
{
	// TODO: probably need to ungrab before this event
	// wlr_signal_emit_safe(&surface->events.unmap, surface);

	// struct sc_layer_surface_v1_configure *configure, *tmp;
	// wl_list_for_each_safe(configure, tmp, &surface->configure_list, link) {
	//	layer_surface_configure_destroy(configure);
	// }

	surface->configured = surface->mapped = false;
}

static void
layer_surface_destroy(struct sc_layer_surface_v1 *surface)
{
	if (surface->configured && surface->mapped) {
		layer_surface_unmap(surface);
	}
	// wlr_signal_emit_safe(&surface->events.destroy, surface);
	wl_resource_set_user_data(surface->resource, NULL);
	surface->surface->role_data = NULL;
	//  wl_list_remove(&surface->surface_destroy.link);

	free(surface);
}

static void
layer_surface_resource_destroy(struct wl_resource *resource)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);
	if (surface != NULL) {
		layer_surface_destroy(surface);
	}
}
static const struct wlr_surface_role layer_surface_role;

bool
sc_surface_is_layer_surface(struct wlr_surface *surface)
{
	return surface->role == &layer_surface_role;
}

struct sc_layer_surface_v1 *
sc_layer_surface_v1_from_wlr_surface(struct wlr_surface *surface)
{
	assert(sc_surface_is_layer_surface(surface));
	return (struct sc_layer_surface_v1 *) surface->role_data;
}

static void
layer_surface_role_commit(struct wlr_surface *wlr_surface)
{
	struct sc_layer_surface_v1 *surface =
		sc_layer_surface_v1_from_wlr_surface(wlr_surface);
	if (surface == NULL) {
		return;
	}
	DLOG("layer_surface_role_commit\n");
	surface->current = surface->pending;
	surface->pending.committed = 0;

	if (wlr_surface_has_buffer(surface->surface) && !surface->configured) {
		// wl_resource_post_error(surface->resource,
		//	SC_SHELL_UNSTABLE_V1_ERROR_ALREADY_CONSTRUCTED,
		//	"layer_surface has never been configured");
		// return;
		surface->configured = true;
	}

	if (!surface->added) {
		surface->added = true;
		assert(!surface->configured);
		assert(!surface->mapped);
		wl_signal_emit(&surface->shell->events.new_layer, surface);

		if (!surface->output) {
			struct sc_output *output = sc_compositor_output_at(
				surface->current.position.x, surface->current.position.y);
			surface->output = output->wlr_output;
		}
		// Return early here as the compositor may have closed this layer
		// surface in response to the new_surface event.
		return;
	}

	if (surface->configured && wlr_surface_has_buffer(surface->surface) &&
		!surface->mapped) {
		surface->mapped = true;

		wl_signal_emit(&surface->events.map, surface);
	}
}

static void
layer_surface_role_precommit(struct wlr_surface *wlr_surface)
{
	struct sc_layer_surface_v1 *surface =
		sc_layer_surface_v1_from_wlr_surface(wlr_surface);
	if (surface == NULL) {
		return;
	}

	if (wlr_surface->pending.committed & WLR_SURFACE_STATE_BUFFER &&
		wlr_surface->pending.buffer == NULL) {
		// This is a NULL commit
		if (surface->configured && surface->mapped) {
			layer_surface_unmap(surface);
		}
	}
}

static const struct wlr_surface_role layer_surface_role = {
	.name = "sc_layer_surface_v1",
	.commit = layer_surface_role_commit,
	.precommit = layer_surface_role_precommit,
};

/**
 * creates a layer surface from a wl_surface
 *
 * A layer surface is used to visualize bitmaps generated from
 * the clients. It provides position, scale, rotation attributes as
 * well some rendering capabilities like: background color,
 * borders, shadows.
 */
void
shell_handle_get_layer_surface(struct wl_client *wl_client,
							   struct wl_resource *client_resource, uint32_t id,
							   struct wl_resource *surface_resource,
							   struct wl_resource *output_resource)
{
	struct sc_layer_shell_v1 *shell =
		layer_shell_from_resource(client_resource);
	struct wlr_surface *wlr_surface =
		wlr_surface_from_resource(surface_resource);

	struct sc_layer_surface_v1 *surface =
		calloc(1, sizeof(struct sc_layer_surface_v1));

	if (surface == NULL) {
		wl_client_post_no_memory(wl_client);
		return;
	}

  surface->pending.opacity = 1.0;

	if (!wlr_surface_set_role(wlr_surface, &layer_surface_role, surface,
							  client_resource,
							  SC_SHELL_UNSTABLE_V1_ERROR_ROLE)) {
		free(surface);
		return;
	}

	surface->shell = shell;
	surface->surface = wlr_surface;
	if (output_resource) {
		surface->output = wlr_output_from_resource(output_resource);
	}

	// surface->current.layer = surface->pending.layer = layer;

	surface->resource =
		wl_resource_create(wl_client, &sc_layer_surface_v1_interface,
						   wl_resource_get_version(client_resource), id);
	if (surface->resource == NULL) {
		free(surface);
		wl_client_post_no_memory(wl_client);
		return;
	}

	wl_signal_init(&surface->events.destroy);
	wl_signal_init(&surface->events.map);
	wl_signal_init(&surface->events.unmap);
	wl_signal_init(&surface->events.add_animation);
	wl_signal_init(&surface->events.animation_update);

	//	wl_signal_add(&surface->surface->events.destroy,
	//		&surface->surface_destroy);
	//	surface->surface_destroy.notify = handle_surface_destroyed;

	LOG("new layer_surface %p (res %p)\n", surface, surface->resource);
	wl_resource_set_implementation(surface->resource,
								   &sc_layer_surface_implementation, surface,
								   layer_surface_resource_destroy);
}

// LAYER STATE

void
layer_handle_set_compositing_filter(struct wl_client *client,
									struct wl_resource *resource,
									struct wl_resource *filter)
{
}
/**
 * Appends the layer to the layer’s list of sublayers
 *
 *
 */
void
layer_handle_add_sublayer(struct wl_client *client,
						  struct wl_resource *resource,
						  struct wl_resource *layer)
{
}

/**
 * Inserts the specified layer into the receiver’s list of sublayers at the
 * specified index
 *
 *
 */
void
layer_handle_insert_sublayer_at_index(struct wl_client *client,
									  struct wl_resource *resource,
									  struct wl_resource *layer, int32_t index)
{
}

/**
 * Detaches the layer from its parent layer
 *
 *
 */
void
layer_handle_remove_from_superlayer(struct wl_client *client,
									struct wl_resource *resource)
{
}

/**
 * The layer’s bounds rectangle. Animatable.
 *
 *
 */
static void
layer_handle_set_bounds(struct wl_client *client, struct wl_resource *resource,
						wl_fixed_t x, wl_fixed_t y, wl_fixed_t width,
						wl_fixed_t height)
{
	LOG("layer_handle_set_bounds\n");

	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.bounds = (struct wlr_fbox){
		.x = wl_fixed_to_double(x),
		.y = wl_fixed_to_double(y),
		.width = wl_fixed_to_double(width),
		.height = wl_fixed_to_double(height),
	};
}

static void
layer_handle_set_position(struct wl_client *client,
						  struct wl_resource *resource, wl_fixed_t x,
						  wl_fixed_t y)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.position.x = wl_fixed_to_double(x);
	surface->pending.position.y = wl_fixed_to_double(y);
}

static void
layer_handle_set_z_position(struct wl_client *client,
							struct wl_resource *resource, wl_fixed_t z)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.z_position = wl_fixed_to_double(z);
}

static void
layer_handle_set_anchor_point(struct wl_client *client,
							  struct wl_resource *resource, wl_fixed_t x,
							  wl_fixed_t y)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.anchor_point.x = wl_fixed_to_double(x);
	surface->pending.anchor_point.y = wl_fixed_to_double(y);
}

static void
layer_handle_set_content_scale(struct wl_client *client,
							   struct wl_resource *resource, wl_fixed_t x,
							   wl_fixed_t y)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.content_scale.x = wl_fixed_to_double(x);
	surface->pending.content_scale.y = wl_fixed_to_double(y);
}

static void
layer_handle_set_opacity(struct wl_client *client, struct wl_resource *resource,
						 wl_fixed_t opacity)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.opacity = wl_fixed_to_double(opacity);
}

static void
layer_handle_set_hidden(struct wl_client *client, struct wl_resource *resource,
						uint32_t hidden)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.hidden = hidden > 0;
}

static void
layer_handle_set_corner_radius(struct wl_client *client,
							   struct wl_resource *resource, wl_fixed_t radius)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.border_corner_radius = wl_fixed_to_double(radius);
}

static void
layer_handle_set_border_width(struct wl_client *client,
							  struct wl_resource *resource, wl_fixed_t width)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.border_width = wl_fixed_to_double(width);
}

static void
layer_handle_set_border_color(struct wl_client *client,
							  struct wl_resource *resource, uint32_t r,
							  uint32_t g, uint32_t b, uint32_t a)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.border_color = (struct sc_color){
		.r = r,
		.g = g,
		.b = b,
		.a = a,
	};
}

static void
layer_handle_set_background_color(struct wl_client *client,
								  struct wl_resource *resource, uint32_t r,
								  uint32_t g, uint32_t b, uint32_t a)
{
	struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

	if (!surface) {
		return;
	}
	surface->pending.background_color = (struct sc_color){
		.r = r,
		.g = g,
		.b = b,
		.a = a,
	};
}
/**
 *
 *
 *
 */
void
layer_handle_set_shadow_opacity(struct wl_client *client,
								struct wl_resource *resource,
								wl_fixed_t opacity)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_shadow_radius(struct wl_client *client,
							   struct wl_resource *resource, uint32_t radius)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_shadow_offset(struct wl_client *client,
							   struct wl_resource *resource, wl_fixed_t x,
							   wl_fixed_t y)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_shadow_color(struct wl_client *client,
							  struct wl_resource *resource, uint32_t red,
							  uint32_t green, uint32_t blue, uint32_t alpha)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_transform(struct wl_client *client,
						   struct wl_resource *resource, wl_fixed_t m11,
						   wl_fixed_t m12, wl_fixed_t m13, wl_fixed_t m14,
						   wl_fixed_t m21, wl_fixed_t m22, wl_fixed_t m23,
						   wl_fixed_t m24, wl_fixed_t m31, wl_fixed_t m32,
						   wl_fixed_t m33, wl_fixed_t m34, wl_fixed_t m41,
						   wl_fixed_t m42, wl_fixed_t m43, wl_fixed_t m44)
{
}

/**
 * Add the specified animation object to the layer.
 *
 * The animation needs to have a value provider assigned before
 * being added.
 */
void
layer_handle_add_animation(struct wl_client *client,
						   struct wl_resource *layer_resource,
						   struct wl_resource *animation_resource,
						   const char *for_key)
{
	struct sc_layer_surface_v1 *layer =
		layer_surface_from_resource(layer_resource);
	struct sc_animation_v1 *animation =
		animation_from_resource(animation_resource);

	if (!layer) {
		return;
	}
	if (!animation) {
		return;
	}
	struct timespec now;
	clockid_t presentation_clock =
		wlr_backend_get_presentation_clock(layer->output->backend);
	clock_gettime(presentation_clock, &now);

	animation->begin_time = timespec_to_double(now);
  animation->is_running = true;

	if (strcmp(for_key, "opacity") == 0) {
    animation->key = &layer->pending.opacity;
		layer->animators.opacity = animation;
	} else if (strcmp(for_key, "corner-radius") == 0) {
    animation->key = &layer->pending.border_corner_radius;
		layer->animators.border_corner_radius = animation;
	} else if (strcmp(for_key, "border-width") == 0) {
    animation->key = &layer->pending.border_width;
		layer->animators.border_width = animation;
	}

	wl_signal_emit(&layer->events.add_animation, animation);
}

/**
 * Remove the animation object with the specified key.
 *
 *
 */
void
layer_handle_remove_animation(struct wl_client *client,
							  struct wl_resource *resource, const char *for_key)
{
}

/**
 * Remove all animations attached to the layer.
 *
 *
 */
void
layer_handle_remove_all_animations(struct wl_client *client,
								   struct wl_resource *resource)
{
}

/**
 * destroy the layer_surface
 *
 * This request destroys the layer surface.
 */
void
layer_handle_destroy(struct wl_client *client, struct wl_resource *resource)
{
}

void
layer_update_animations(struct sc_layer_surface_v1 *layer, double timestamp)
{
	if (layer->animators.opacity && layer->animators.opacity->is_running) {
		layer->pending.opacity = *((double *) animation_value_at(
			layer->animators.opacity, timestamp));

    wl_signal_emit(&layer->events.animation_update, layer->animators.opacity);
	}
	if (layer->animators.border_corner_radius && layer->animators.border_corner_radius->is_running) {
		layer->pending.border_corner_radius = *((double *) animation_value_at(
			layer->animators.border_corner_radius, timestamp));

    wl_signal_emit(&layer->events.animation_update, layer->animators.border_corner_radius);
	}
	if (layer->animators.border_width && layer->animators.border_width->is_running) {
		layer->pending.border_width = *((double *) animation_value_at(
			layer->animators.border_width, timestamp));

    wl_signal_emit(&layer->events.animation_update, layer->animators.border_width);
	}
	layer->current = layer->pending;
}
static const struct sc_layer_surface_v1_interface
	sc_layer_surface_implementation = {
		.set_compositing_filter = layer_handle_set_compositing_filter,
		.add_sublayer = layer_handle_add_sublayer,
		.insert_sublayer_at_index = layer_handle_insert_sublayer_at_index,
		.remove_from_superlayer = layer_handle_remove_from_superlayer,
		.set_bounds = layer_handle_set_bounds,
		.set_position = layer_handle_set_position,
		.set_z_position = layer_handle_set_z_position,
		.set_anchor_point = layer_handle_set_anchor_point,
		.set_content_scale = layer_handle_set_content_scale,
		.set_opacity = layer_handle_set_opacity,
		.set_hidden = layer_handle_set_hidden,
		.set_corner_radius = layer_handle_set_corner_radius,
		.set_border_width = layer_handle_set_border_width,
		.set_border_color = layer_handle_set_border_color,
		.set_background_color = layer_handle_set_background_color,
		.set_shadow_opacity = layer_handle_set_shadow_opacity,
		.set_shadow_radius = layer_handle_set_shadow_radius,
		.set_shadow_offset = layer_handle_set_shadow_offset,
		.set_shadow_color = layer_handle_set_shadow_color,
		.set_transform = layer_handle_set_transform,
		.add_animation = layer_handle_add_animation,
		.remove_animation = layer_handle_remove_animation,
		.remove_all_animations = layer_handle_remove_all_animations,
		.destroy = layer_handle_destroy,
};
