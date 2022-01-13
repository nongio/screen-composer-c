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

static const struct sc_shell_unstable_v1_interface sc_layer_shell_implementation
  = {
    .get_layer_surface = layer_shell_handle_get_layer_surface,
    //.get_animation = layer_shell_handle_get_animation,
    .destroy = resource_handle_destroy,
};

