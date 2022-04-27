#ifndef _SC_LAYER_SHELL_H
#define _SC_LAYER_SHELL_H

#include "sc-layer-unstable-v1-protocol.h"
#include <wlr/util/box.h>

struct sc_point {
	float x;
	float y;
};

struct sc_rect {
	float x;
	float y;
	float width;
	float height;
};

struct sc_color {
	uint32_t r;
	uint32_t g;
	uint32_t b;
	uint32_t a;
};

struct sc_matrix {
	float m11;
	float m12;
	float m13;
	float m14;
	float m21;
	float m22;
	float m23;
	float m24;
	float m31;
	float m32;
	float m33;
	float m34;
	float m41;
	float m42;
	float m43;
	float m44;
};

struct sc_layer_shell_v1 {
	struct wl_global *global;

	struct wl_listener display_destroy;

	struct {
		struct wl_signal new_layer;
		struct wl_signal new_animation;
		struct wl_signal destroy;
	} events;

	void *data;
};

struct sc_layer_v1_state {
	uint32_t committed;

	struct wlr_fbox bounds;
	float z_position;
	struct sc_point position;
	struct sc_point anchor_point;
	struct sc_point content_scale;
	float opacity;
	bool hidden;
	float border_corner_radius;
	float border_width;
	struct sc_color border_color;
	struct sc_color background_color;

	uint32_t configure_serial;
};

struct sc_layer_surface_v1 {
	struct wlr_surface *surface;
	bool added, configured, mapped;
	struct wlr_output *output;
	struct wl_resource *resource;
	struct sc_layer_shell_v1 *shell;

	struct sc_layer_v1_state current, pending;

	struct wl_listener surface_destroy;

	struct {
		struct wl_signal destroy;
		struct wl_signal map;
		struct wl_signal unmap;
	} events;

	void *data;
};

typedef enum sc_animation_v1_animation_value_type SCAnimationValueType;

struct sc_timing_function_v1 {
	struct wl_resource *resource;
	struct sc_layer_shell_v1 *shell;

	float cx0;
	float cy0;
	float cx1;
	float cy1;
	float cx2;
	float cy2;
	float cx3;
	float cy3;

	struct {
		struct wl_signal destroy;
	} events;
};

struct sc_animation_impl {
	void (*value)(double fraction);
};

struct sc_animation_v1 {
	struct wl_resource *resource;
	struct sc_layer_shell_v1 *shell;

	char *keypath;

	const struct sc_animation_impl *impl;

	double duration;
	double begin_time;
	float speed;
	float repeat_count;

	bool autoreverse;

	struct {
		struct wl_signal destroy;
	} events;
};

struct sc_basic_animation_v1 {
	struct wl_resource *resource;
	struct sc_layer_shell_v1 *shell;
	struct sc_animation_v1 *animation;

	enum sc_animation_v1_animation_value_type type;

	void *from_value;
	void *by_value;
	void *to_value;

	struct sc_timing_function_v1 *timing_function;

	struct {
		struct wl_signal destroy;
	} events;
};

struct sc_layer_shell_v1 *sc_layer_shell_v1_create(struct wl_display *display);

struct sc_layer_shell_v1 *
layer_shell_from_resource(struct wl_resource *resource);

struct sc_animation_v1 *animation_from_resource(struct wl_resource *resource);

#endif
