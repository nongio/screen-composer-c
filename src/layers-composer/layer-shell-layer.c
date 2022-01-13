#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <wayland-server-core.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_surface.h>

#include "log.h"
#include "sc-layer-unstable-v1-protocol.h"
#include "sc-layer-shell-layer.h"
#include "sc-layer-shell.h"

static const struct sc_layer_surface_v1_interface
  sc_layer_surface_implementation;


static struct sc_layer_surface_v1 *
layer_surface_from_resource(struct wl_resource *resource)
{
  assert(wl_resource_instance_of(resource, &sc_layer_surface_v1_interface,
				 &sc_layer_surface_implementation));
  return wl_resource_get_user_data(resource);
}

static void layer_surface_unmap(struct sc_layer_surface_v1 *surface) {
	// TODO: probably need to ungrab before this event
	// wlr_signal_emit_safe(&surface->events.unmap, surface);

	//struct sc_layer_surface_v1_configure *configure, *tmp;
	//wl_list_for_each_safe(configure, tmp, &surface->configure_list, link) {
	//	layer_surface_configure_destroy(configure);
	//}

	surface->configured = surface->mapped = false;
}

static void
layer_surface_destroy(struct sc_layer_surface_v1 *surface)
{
  if (surface->configured && surface->mapped) {
  	layer_surface_unmap(surface);
  }
  //wlr_signal_emit_safe(&surface->events.destroy, surface);
  wl_resource_set_user_data(surface->resource, NULL);
  surface->surface->role_data = NULL;
  //  wl_list_remove(&surface->surface_destroy.link);

  free(surface);
}

static void
layer_surface_resource_destroy(struct wl_resource *resource)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);
  if (surface != NULL)
    {
      layer_surface_destroy(surface);
    }
}
static const struct wlr_surface_role layer_surface_role;

bool sc_surface_is_layer_surface(struct wlr_surface *surface) {
	return surface->role == &layer_surface_role;
}

struct sc_layer_surface_v1 *sc_layer_surface_v1_from_wlr_surface(
		struct wlr_surface *surface) {
	assert(sc_surface_is_layer_surface(surface));
	return (struct sc_layer_surface_v1 *)surface->role_data;
}

static void layer_surface_role_commit(struct wlr_surface *wlr_surface) {
	struct sc_layer_surface_v1 *surface =
		sc_layer_surface_v1_from_wlr_surface(wlr_surface);
	if (surface == NULL) {
		return;
	}
  DLOG("layer_surface_role_commit\n");
	surface->current = surface->pending;
	surface->pending.committed = 0;

	if (wlr_surface_has_buffer(surface->surface) && !surface->configured) {
		//wl_resource_post_error(surface->resource,
		//	SC_SHELL_UNSTABLE_V1_ERROR_ALREADY_CONSTRUCTED,
		//	"layer_surface has never been configured");
		//return;
    surface->configured = true;
	}

	if (!surface->added) {
		surface->added = true;
		assert(!surface->configured);
		assert(!surface->mapped);
		wl_signal_emit(&surface->shell->events.new_layer, surface);
		// Return early here as the compositor may have closed this layer surface
		// in response to the new_surface event.
		return;
	}

	if (surface->configured && wlr_surface_has_buffer(surface->surface) &&
			!surface->mapped) {
		surface->mapped = true;
		wl_signal_emit(&surface->events.map, surface);
	}
}

static void layer_surface_role_precommit(struct wlr_surface *wlr_surface) {
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

void
layer_shell_handle_get_layer_surface(struct wl_client   *wl_client,
				     struct wl_resource *client_resource,
				     uint32_t		 id,
				     struct wl_resource *surface_resource,
				     struct wl_resource *output_resource)
{
  struct sc_layer_shell_v1 *shell = layer_shell_from_resource(client_resource);
  struct wlr_surface *wlr_surface = wlr_surface_from_resource(surface_resource);

  struct sc_layer_surface_v1 *surface
    = calloc(1, sizeof(struct sc_layer_surface_v1));
  if (surface == NULL)
    {
      wl_client_post_no_memory(wl_client);
      return;
    }

  if (!wlr_surface_set_role(wlr_surface, &layer_surface_role, surface,
   	client_resource, SC_SHELL_UNSTABLE_V1_ERROR_ROLE)) {
   free(surface);
   return;
  }

  surface->shell = shell;
  surface->surface = wlr_surface;
  if (output_resource)
    {
      surface->output = wlr_output_from_resource(output_resource);
    }

  //surface->current.layer = surface->pending.layer = layer;

  surface->resource
    = wl_resource_create(wl_client, &sc_layer_surface_v1_interface,
			 wl_resource_get_version(client_resource), id);
  if (surface->resource == NULL)
    {
      free(surface);
      wl_client_post_no_memory(wl_client);
      return;
    }

  wl_signal_init(&surface->events.destroy);
  wl_signal_init(&surface->events.map);
  wl_signal_init(&surface->events.unmap);

  //	wl_signal_add(&surface->surface->events.destroy,
  //		&surface->surface_destroy);
  //	surface->surface_destroy.notify = handle_surface_destroyed;

  DLOG("new layer_surface %p (res %p)\n", surface, surface->resource);
  wl_resource_set_implementation(surface->resource,
				 &sc_layer_surface_implementation, surface,
				 layer_surface_resource_destroy);
}


// LAYER STATE

static void
layer_surface_handle_set_bounds(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t x,
			       wl_fixed_t y, wl_fixed_t width, wl_fixed_t height)
{
  DLOG("layer_surface_handle_set_bounds\n");

  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.bounds = (struct wlr_fbox) {
    .x = wl_fixed_to_double(x),
    .y = wl_fixed_to_double(y),
    .width = wl_fixed_to_double(width),
    .height = wl_fixed_to_double(height),
  };
}

static void
layer_surface_handle_set_position(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t x,
			       wl_fixed_t y)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.position.x = wl_fixed_to_double(x);
  surface->pending.position.y = wl_fixed_to_double(y);
}

static void
layer_surface_handle_set_z_position(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t z)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.z_position = wl_fixed_to_double(z);
}

static void
layer_surface_handle_set_anchor(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t x, wl_fixed_t y)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.anchor_point.x = wl_fixed_to_double(x);
  surface->pending.anchor_point.y = wl_fixed_to_double(y);
}

static void
layer_surface_handle_set_content_scale(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t x, wl_fixed_t y)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.content_scale.x = wl_fixed_to_double(x);
  surface->pending.content_scale.y = wl_fixed_to_double(y);
}

static void
layer_surface_handle_set_opacity(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t opacity)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.opacity = wl_fixed_to_double(opacity);
}

static void
layer_surface_handle_set_hidden(struct wl_client	*client,
			       struct wl_resource *resource, uint32_t hidden)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.hidden = hidden > 0;
}

static void
layer_surface_handle_set_corner_radius(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t radius)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.border_corner_radius = wl_fixed_to_double(radius);
}

static void
layer_surface_handle_set_border_width(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t width)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.border_width = wl_fixed_to_double(width);
}

static void
layer_surface_handle_set_border_color(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t r, wl_fixed_t g, wl_fixed_t b, wl_fixed_t a)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.border_color = (struct sc_color){
    .r = wl_fixed_to_double(r),
    .g = wl_fixed_to_double(g),
    .b = wl_fixed_to_double(b),
    .a = wl_fixed_to_double(a),
  };
}

static void
layer_surface_handle_set_background_color(struct wl_client	*client,
			       struct wl_resource *resource, wl_fixed_t r, wl_fixed_t g, wl_fixed_t b, wl_fixed_t a)
{
  struct sc_layer_surface_v1 *surface = layer_surface_from_resource(resource);

  if (!surface)
    {
      return;
    }
  surface->pending.background_color = (struct sc_color){
    .r = wl_fixed_to_double(r),
    .g = wl_fixed_to_double(g),
    .b = wl_fixed_to_double(b),
    .a = wl_fixed_to_double(a),
  };
}


static const struct sc_layer_surface_v1_interface
  sc_layer_surface_implementation
  = {
    .set_bounds = layer_surface_handle_set_bounds,
    .set_position = layer_surface_handle_set_position,
    .set_z_position = layer_surface_handle_set_z_position,
    .set_anchor_point = layer_surface_handle_set_anchor,
    .set_content_scale = layer_surface_handle_set_content_scale,
    .set_opacity = layer_surface_handle_set_opacity,
    .set_hidden = layer_surface_handle_set_hidden,
    .set_corner_radius = layer_surface_handle_set_corner_radius,
    .set_border_width = layer_surface_handle_set_border_width,
    .set_border_color = layer_surface_handle_set_border_color,
    .set_background_color = layer_surface_handle_set_background_color,
};
