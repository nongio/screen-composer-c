#ifndef _SC_LAYER_SHELL_ANIMATION_H
#define _SC_LAYER_SHELL_ANIMATION_H

void shell_handle_get_animation(struct wl_client *wl_client,
				struct wl_resource *client_resource,
				uint32_t id,
				wl_fixed_t duration,
				wl_fixed_t speed,
				int32_t repeat_count,
				uint32_t autoreverse);

#endif


