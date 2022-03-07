#define _POSIX_C_SOURCE 200809L
#include "sc-layer-unstable-v1-protocol.h"
#include <wayland-server-core.h>

static unsigned int S_FLOAT = sizeof(float);

#define FLOAT_ARRAY_AT(array, index) ((float *) (array->data + index * S_FLOAT))

float *
decode_value(struct wl_array *array)
{
	if (array->size < S_FLOAT) {
		return NULL;
	}

	float *value = malloc(S_FLOAT);
	*value = *FLOAT_ARRAY_AT(array, 0);
	return value;
}

struct sc_point *
decode_point(struct wl_array *array)
{
	if (array->size < S_FLOAT * 2) {
		return NULL;
	}
	struct sc_point *value = malloc(sizeof(struct sc_point));
	value->x = *FLOAT_ARRAY_AT(array, 0);
	value->y = *FLOAT_ARRAY_AT(array, 1);
	DLOG("decode point: %f, %f\n", value->x, value->y);
	return value;
}

struct sc_rect *
decode_rect(struct wl_array *array)
{
	if (array->size < S_FLOAT * 4) {
		return NULL;
	}
	struct sc_rect *value = malloc(sizeof(struct sc_rect));
	value->x = *FLOAT_ARRAY_AT(array, 0);
	value->y = *FLOAT_ARRAY_AT(array, 1);
	value->width = *FLOAT_ARRAY_AT(array, 2);
	value->height = *FLOAT_ARRAY_AT(array, 3);
	return value;
}

struct sc_color *
decode_color(struct wl_array *array)
{
	if (array->size < S_FLOAT * 4) {
		return NULL;
	}
	struct sc_color *value = malloc(sizeof(struct sc_color));
	value->r = *FLOAT_ARRAY_AT(array, 0);
	value->g = *FLOAT_ARRAY_AT(array, 1);
	value->b = *FLOAT_ARRAY_AT(array, 2);
	value->a = *FLOAT_ARRAY_AT(array, 3);
	return value;
}

struct sc_matrix *
decode_matrix(struct wl_array *array)
{
	if (array->size < S_FLOAT * 16) {
		return NULL;
	}
	struct sc_matrix *value = malloc(sizeof(struct sc_matrix));
	value->m11 = *FLOAT_ARRAY_AT(array, 0);
	value->m12 = *FLOAT_ARRAY_AT(array, 1);
	value->m13 = *FLOAT_ARRAY_AT(array, 2);
	value->m14 = *FLOAT_ARRAY_AT(array, 3);
	value->m21 = *FLOAT_ARRAY_AT(array, 4);
	value->m22 = *FLOAT_ARRAY_AT(array, 5);
	value->m23 = *FLOAT_ARRAY_AT(array, 6);
	value->m24 = *FLOAT_ARRAY_AT(array, 7);
	value->m31 = *FLOAT_ARRAY_AT(array, 8);
	value->m32 = *FLOAT_ARRAY_AT(array, 9);
	value->m33 = *FLOAT_ARRAY_AT(array, 10);
	value->m34 = *FLOAT_ARRAY_AT(array, 11);
	value->m41 = *FLOAT_ARRAY_AT(array, 12);
	value->m42 = *FLOAT_ARRAY_AT(array, 13);
	value->m43 = *FLOAT_ARRAY_AT(array, 14);
	value->m44 = *FLOAT_ARRAY_AT(array, 15);
	return value;
}

void *
decode_animation_value_for_type(
	struct wl_array *data,
	enum sc_animation_v1_animation_value_type animation_type)
{
	void *decoded_value = NULL;
	switch (animation_type) {
	case SC_ANIMATION_V1_ANIMATION_VALUE_TYPE_VALUE:
		decoded_value = decode_value(data);
		break;
	case SC_ANIMATION_V1_ANIMATION_VALUE_TYPE_POINT:
		decoded_value = decode_point(data);
		break;
	case SC_ANIMATION_V1_ANIMATION_VALUE_TYPE_RECT:
		decoded_value = decode_rect(data);
		break;
	case SC_ANIMATION_V1_ANIMATION_VALUE_TYPE_COLOR:
		decoded_value = decode_color(data);
		break;
	case SC_ANIMATION_V1_ANIMATION_VALUE_TYPE_MATRIX:
		decoded_value = decode_matrix(data);
		break;
	}

	return decoded_value;
}
