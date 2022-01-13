#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <wayland-server-core.h>

#include "log.h"
#include "sc-layer-unstable-v1-protocol.h"
#include "sc-layer-shell-layer.h"
#include "sc-layer-shell.h"
#include "sc-layer-shell-animation.h"


static const struct sc_animation_v1_interface
  sc_animation_implementation;

static void
animation_destroy(struct sc_animation_v1 *animation)
{
	wl_signal_emit(&animation->events.destroy, animation);

  wl_resource_set_user_data(animation->resource, NULL);

  free(animation);
}

static struct sc_animation_v1 *
animation_from_resource(struct wl_resource *resource)
{
  assert(wl_resource_instance_of(resource, &sc_animation_v1_interface,
				 &sc_animation_implementation));
  return wl_resource_get_user_data(resource);
}

static void
animation_resource_destroy(struct wl_resource *resource)
{
  struct sc_animation_v1 *animation = animation_from_resource(resource);
  if (animation != NULL)
    {
      animation_destroy(animation);
    }
}

void
layer_shell_handle_get_animation(struct wl_client	  *wl_client,
				 struct wl_resource *resource, uint32_t id,
				 uint32_t value_type)
{
  struct sc_layer_shell_v1 *shell = layer_shell_from_resource(resource);

  struct sc_animation_v1 *animation
    = calloc(1, sizeof(struct sc_animation_v1));

  animation->timing_function = calloc(1, sizeof(struct sc_timing_function_v1));
  // easeout
  animation->timing_function->cx0 = 0.0;
  animation->timing_function->cy0 = 0.0;
  animation->timing_function->cx1 = 0.0;
  animation->timing_function->cy1 = 0.0;
  animation->timing_function->cx2 = 0.58;
  animation->timing_function->cy2 = 1.0;
  animation->timing_function->cx3 = 1.0;
  animation->timing_function->cy3 = 1.0;

  if (animation == NULL)
    {
      wl_client_post_no_memory(wl_client);
      return;
    }

  animation->shell = shell;

  animation->resource
    = wl_resource_create(wl_client, &sc_animation_v1_interface,
			 wl_resource_get_version(resource), id);

  if (animation->resource == NULL)
    {
      free(animation);
      wl_client_post_no_memory(wl_client);
      return;
    }

  animation->type = (enum sc_animation_v1_animation_value_type)value_type;
  wl_signal_init(&animation->events.destroy);

  DLOG("new animation %p (res %p)\n", animation, animation->resource);

  wl_resource_set_implementation(animation->resource,
				 &sc_animation_implementation, animation,
				 animation_resource_destroy);
}

static void
animation_handle_set_keypath(struct wl_client   *client,
			     struct wl_resource *resource, const char *keypath)
{
  struct sc_animation_v1 *animation = animation_from_resource(resource);

  if (!animation)
    {
      return;
    }
  animation->keypath = strdup(keypath);
}
static unsigned int S_FLOAT = sizeof(float);

#define FLOAT_ARRAY_AT(array, index) ((float*)(array->data + index*S_FLOAT))

static float * decode_value(struct wl_array *array)
{
  if (array->size < S_FLOAT)
    {
      return NULL;
    }

  float * value = malloc(S_FLOAT);
  *value = *FLOAT_ARRAY_AT(array, 0);
  return value;

}

static struct sc_point * decode_point(struct wl_array *array)
{
  if (array->size < S_FLOAT * 2)
    {
      return NULL;
    }
  struct sc_point * value = malloc(sizeof(struct sc_point));
  value->x = *FLOAT_ARRAY_AT(array, 0);
  value->y = *FLOAT_ARRAY_AT(array, 1);
  DLOG("decode point: %f, %f\n", value->x, value->y);
  return value;
}

static struct sc_rect * decode_rect(struct wl_array *array)
{
  if (array->size < S_FLOAT * 4)
    {
      return NULL;
    }
  struct sc_rect * value = malloc(sizeof(struct sc_rect));
  value->x = *FLOAT_ARRAY_AT(array, 0);
  value->y = *FLOAT_ARRAY_AT(array, 1);
  value->width = *FLOAT_ARRAY_AT(array, 2);
  value->height = *FLOAT_ARRAY_AT(array, 3);
  return value;
}

static struct sc_color * decode_color(struct wl_array *array)
{
  if (array->size < S_FLOAT * 4)
    {
      return NULL;
    }
  struct sc_color * value = malloc(sizeof(struct sc_color));
  value->r = *FLOAT_ARRAY_AT(array, 0);
  value->g = *FLOAT_ARRAY_AT(array, 1);
  value->b = *FLOAT_ARRAY_AT(array, 2);
  value->a = *FLOAT_ARRAY_AT(array, 3);
  return value;
}

static struct sc_matrix * decode_matrix(struct wl_array *array)
{
  if (array->size < S_FLOAT * 16)
    {
      return NULL;
    }
  struct sc_matrix * value = malloc(sizeof(struct sc_matrix));
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

static void * decode_animation_value_for_type(struct wl_array *data, enum sc_animation_v1_animation_value_type animation_type)
{
  void * decoded_value = NULL;
  switch(animation_type)
    {
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
void
animation_handle_set_from_value(struct wl_client	 *client,
				struct wl_resource *resource,
				struct wl_array	*from_value)
{
  struct sc_animation_v1 *animation = animation_from_resource(resource);

  if (!animation)
    {
      return;
    }
  animation->from_value = decode_animation_value_for_type(from_value, animation->type);
}

void
animation_handle_set_to_value(struct wl_client   *client,
			      struct wl_resource *resource,
			      struct wl_array    *to_value)
{
  struct sc_animation_v1 *animation = animation_from_resource(resource);

  if (!animation)
    {
      return;
    }
  animation->to_value = decode_animation_value_for_type(to_value, animation->type);


  wl_signal_emit(&animation->shell->events.new_animation, animation);
}

void
animation_handle_set_by_value(struct wl_client   *client,
			      struct wl_resource *resource,
			      struct wl_array    *by_value)
{
  struct sc_animation_v1 *animation = animation_from_resource(resource);

  if (!animation)
    {
      return;
    }
  animation->by_value = decode_animation_value_for_type(by_value, animation->type);
}

static void
animation_handle_set_cumulative(struct wl_client	 *client,
				struct wl_resource *resource,
				uint32_t	    cumulative)
{}

static void
animation_handle_set_additive(struct wl_client   *client,
			      struct wl_resource *resource, uint32_t additive)
{}
static void
animation_handle_set_removed_on_completion(struct wl_client   *client,
					   struct wl_resource *resource,
					   uint32_t removed_on_completion)
{}

void
animation_handle_set_timing_function(struct wl_client   *client,
				     struct wl_resource *resource,
				     struct wl_resource *timing_function)
{}

static const struct sc_animation_v1_interface
  sc_animation_implementation
  = {
    .set_keypath = animation_handle_set_keypath,
    //.set_from_value = animation_handle_set_from_value,
    //.set_to_value = animation_handle_set_to_value,
    //.set_by_value = animation_handle_set_by_value,
    .set_cumulative = animation_handle_set_cumulative,
    .set_additive = animation_handle_set_additive,
    .set_removed_on_completion = animation_handle_set_removed_on_completion,
    //.set_timing_function = animation_handle_set_timing_function,
};

