#define _POSIX_C_SOURCE 200809L

#include "sc-layer-shell.h"

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
void
layer_handle_set_bounds(struct wl_client *client, struct wl_resource *resource,
						wl_fixed_t x, wl_fixed_t y, wl_fixed_t width,
						wl_fixed_t height)
{
}

/**
 * The layer’s position in its superlayer’s coordinate space. Animatable.
 *
 *
 */
void
layer_handle_set_position(struct wl_client *client,
						  struct wl_resource *resource, wl_fixed_t x,
						  wl_fixed_t y)
{
}

/**
 * The layer’s position on the z axis. Animatable.
 *
 *
 */
void
layer_handle_set_z_position(struct wl_client *client,
							struct wl_resource *resource, wl_fixed_t z)
{
}

/**
 * Defines the anchor point of the layer's bounds rectangle. Animatable.
 *
 *
 */
void
layer_handle_set_anchor_point(struct wl_client *client,
							  struct wl_resource *resource, wl_fixed_t x,
							  wl_fixed_t y)
{
}

/**
 * Defines the anchor point of the layer's bounds rectangle. Animatable.
 *
 *
 */
void
layer_handle_set_content_scale(struct wl_client *client,
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
layer_handle_set_opacity(struct wl_client *client, struct wl_resource *resource,
						 wl_fixed_t opacity)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_hidden(struct wl_client *client, struct wl_resource *resource,
						uint32_t hidden)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_corner_radius(struct wl_client *client,
							   struct wl_resource *resource, wl_fixed_t radius)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_border_width(struct wl_client *client,
							  struct wl_resource *resource, wl_fixed_t width)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_border_color(struct wl_client *client,
							  struct wl_resource *resource, wl_fixed_t red,
							  wl_fixed_t green, wl_fixed_t blue,
							  wl_fixed_t alpha)
{
}

/**
 *
 *
 *
 */
void
layer_handle_set_background_color(struct wl_client *client,
								  struct wl_resource *resource, wl_fixed_t red,
								  wl_fixed_t green, wl_fixed_t blue,
								  wl_fixed_t alpha)
{
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
							  struct wl_resource *resource, wl_fixed_t red,
							  wl_fixed_t green, wl_fixed_t blue,
							  wl_fixed_t alpha)
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
						   struct wl_resource *resource,
						   struct wl_resource *animation, const char *for_key)
{
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

struct sc_layer_surface_v1_interface sc_layer_surface_implementation = {
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

