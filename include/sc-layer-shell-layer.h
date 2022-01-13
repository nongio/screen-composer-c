#ifndef _SCLayerShelLayer_
#define _SCLayerShelLayer_

void
layer_shell_handle_get_layer_surface(struct wl_client   *wl_client,
				     struct wl_resource *client_resource,
				     uint32_t		 id,
				     struct wl_resource *surface_resource,
				     struct wl_resource *output_resource);

#endif

