#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <wayland-server-core.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_surface.h>
#include <wlr/types/wlr_xdg_shell.h>

#include "log.h"
#include "sc-layer-shell-animation.h"
#include "sc-layer-shell-layer.h"
#include "sc-layer-unstable-v1-protocol.h"
#include "sc-layer-shell.h"

#define SC_SHELL_VERSION 1

static const struct sc_shell_unstable_v1_interface
  sc_layer_shell_implementation;

void
resource_handle_destroy(struct wl_client *client, struct wl_resource *resource)
{
  wl_resource_destroy(resource);
}

static void
sc_shell_bind(struct wl_client *wl_client, void *data, uint32_t version,
	      uint32_t id)
{
  struct sc__v1 *layer_shell = data;
  assert(wl_client && layer_shell);

  struct wl_resource *resource
    = wl_resource_create(wl_client, &sc_shell_unstable_v1_interface, version,
			 id);
  if (resource == NULL)
    {
      wl_client_post_no_memory(wl_client);
      return;
    }
  wl_resource_set_implementation(resource, &sc_layer_shell_implementation,
				 layer_shell, NULL);
}

struct sc_layer_shell_v1 *
sc_layer_shell_v1_create(struct wl_display *display)
{
  struct sc_layer_shell_v1 *layer_shell
    = malloc(sizeof(struct sc_layer_shell_v1));
  if (!layer_shell)
    {
      return NULL;
    }

  struct wl_global *global
    = wl_global_create(display, &sc_shell_unstable_v1_interface,
		       SC_SHELL_VERSION, layer_shell, sc_shell_bind);
  if (!global)
    {
      free(layer_shell);
      return NULL;
    }
  layer_shell->global = global;

  wl_signal_init(&layer_shell->events.new_layer);
  wl_signal_init(&layer_shell->events.new_animation);
  wl_signal_init(&layer_shell->events.destroy);

  // TODO
  // layer_shell->display_destroy.notify = handle_display_destroy;
  // wl_display_add_destroy_listener(display, &layer_shell->display_destroy);

  return layer_shell;
}

struct sc_layer_shell_v1 *
layer_shell_from_resource(struct wl_resource *resource)
{
  assert(wl_resource_instance_of(resource, &sc_shell_unstable_v1_interface,
				 &sc_layer_shell_implementation));
  return wl_resource_get_user_data(resource);
}

/**
	* an animation value provider, it interpolates between 2 or 3 values
	*
	* The basic animation can interpolate 2 or 3 values using the
	* provided timing function.
	* @param value_type type of values this animation will interpolate
	*/
void shell_handle_get_basic_animation(struct wl_client *client,
				struct wl_resource *resource,
				uint32_t id,
				uint32_t value_type,
				struct wl_resource *animation,
				struct wl_resource *timing)
{

}

/**
	* a timing function described by a bezier curve
	*
	*
	*/
void shell_handle_get_timing_function(struct wl_client *client,
				struct wl_resource *resource,
				uint32_t id)
{

}

/**
	* built-in compositing filter
	*
	*
	* @param name name of the filter
	*/
void shell_handle_get_compositing_filter(struct wl_client *client,
					struct wl_resource *resource,
					uint32_t id,
					const char *name)
{

}

/**
	* destroy the layer object
	*
	* This request indicates that the client will not use the layer
	* object any more. Objects that have been created through this
	* instance are not affected.
	*/
void shell_handle_destroy(struct wl_client *client,
		struct wl_resource *resource)
{

}



static const struct sc_shell_unstable_v1_interface sc_layer_shell_implementation
  = {
    .get_layer_surface = shell_handle_get_layer_surface,
	.get_animation = shell_handle_get_animation,
//    .get_basic_animation = shell_handle_get_basic_animation,
//	.get_timing_function = shell_handle_get_timing_function,
//	.get_compositing_filter = shell_handle_get_compositing_filter,
    .destroy = shell_handle_destroy,
};