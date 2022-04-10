#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/render/gles2.h>
#include <wlr/util/region.h>

#include "gles2_renderer.h"
#include "log.h"
#include "sc_compositor.h"
#include "sc_output.h"
#include "sc_toplevel_view.h"
#include "sc_wlr_layer_view.h"
#include "sc_view.h"
#include "sc_popup_view.h"
#include "sc_workspace.h"
#include "sc_fbo.h"
#include "sc_skia.h"

struct render_data {
	struct sc_output *output;
	pixman_region32_t *damage;
	struct sc_view *view;
};

void
sc_compositor_setup_gles2()
{
	sc_renderer_load_shaders();
}

void
render_surface(struct wlr_surface *surface, int x, int y, void *data)
{

	struct render_data *rdata = data;
	pixman_region32_t *ouput_damage = rdata->damage;
	struct sc_output *output = rdata->output;
	struct sc_view *view = rdata->view;

	int sx = view->frame.x;
	int sy = view->frame.y;

	pixman_region32_t damage;
	pixman_region32_init(&damage);
	pixman_region32_union_rect(&damage, &damage, x, y, surface->current.width,
							   surface->current.height);

	bool damaged = true;
	if (ouput_damage != NULL) {
		pixman_region32_intersect(&damage, &damage, ouput_damage);

		damaged = pixman_region32_not_empty(&damage);
	}
	struct wlr_texture *texture = wlr_surface_get_texture(surface);

	if (damaged && texture != NULL) {
		struct wlr_gles2_texture_attribs *tex_attribs =
			malloc(sizeof(struct wlr_gles2_texture_attribs));
		wlr_gles2_texture_get_attribs(texture, tex_attribs);

		sc_render_texture_with_output(
			tex_attribs, sx + x, sy + y, surface->current.width,
			surface->current.height, surface->current.transform, output);

		free(tex_attribs);
	}
	// damage finish
	pixman_region32_fini(&damage);

	struct timespec repaint_time;

	clockid_t presentation_clock =
		wlr_backend_get_presentation_clock(output->compositor->wlr_backend);

	clock_gettime(presentation_clock, &repaint_time);

	wlr_presentation_surface_sampled_on_output(
		output->compositor->wlr_presentation, surface, output->wlr_output);
}

void
sc_render_output_gl(struct sc_output *output, struct timespec *when,
				 pixman_region32_t *damage)
{
	struct wlr_output *wlr_output = output->wlr_output;
	struct wlr_renderer *renderer = output->compositor->wlr_renderer;

	wlr_renderer_begin(renderer, wlr_output->width, wlr_output->height);

	if (!pixman_region32_not_empty(damage)) {
		// Output isn't damaged but needs buffer swap
		goto renderer_end;
	}

	float clear_color[] = {0.25f, 0.25f, 0.25f, 1.0f};
	glClearColor(clear_color[0], clear_color[1], clear_color[2],
				 clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT);

	struct render_data render_data = {
		.output = output,
		.damage = damage,
	};

	struct sc_toplevel_view *toplevel_view;
	struct sc_workspace *workspace = output->compositor->current_workspace;
	wl_list_for_each_reverse (toplevel_view, &workspace->views_toplevel, link) {

		render_data.view = &toplevel_view->super;
		sc_view_for_each_surface(&toplevel_view->super, render_surface,
								 &render_data);
	}

renderer_end:
	wlr_renderer_scissor(renderer, NULL);
	wlr_output_render_software_cursors(wlr_output, damage);
	wlr_renderer_end(renderer);

	int width, height;
	wlr_output_transformed_resolution(wlr_output, &width, &height);

	pixman_region32_t frame_damage;
	pixman_region32_init(&frame_damage);

	enum wl_output_transform transform =
		wlr_output_transform_invert(wlr_output->transform);
	wlr_region_transform(&frame_damage, &output->damage->current, transform,
						 width, height);

	wlr_output_set_damage(wlr_output, &frame_damage);
	pixman_region32_fini(&frame_damage);

	if (!wlr_output_commit(wlr_output)) {
		return;
	}
	output->last_frame = *when;
}

bool output_box_is_damged(struct sc_output *output, struct wlr_box *box, pixman_region32_t *output_damage)
{
	pixman_region32_t damage;
	pixman_region32_init(&damage);
	pixman_region32_union_rect(&damage, &damage, box->y, box->y, box->width, box->height);

	bool damaged = true;
	if (output_damage != NULL) {
		pixman_region32_intersect(&damage, &damage, output_damage);

		damaged = pixman_region32_not_empty(&damage);
	}
	pixman_region32_fini(&damage);
	return damaged;
}

void
sc_render_view(struct sc_view *view, float x, float y, pixman_region32_t *output_damage)
{
	int sx = x*view->output->wlr_output->scale;
	int sy = y*view->output->wlr_output->scale;

	struct wlr_box box = {
		.x = sx + view->frame.x * view->output->wlr_output->scale,
		.y = sy + view->frame.y * view->output->wlr_output->scale,
		.width = view->frame.width * view->output->wlr_output->scale,
		.height = view->frame.height * view->output->wlr_output->scale,
	};

	if (output_box_is_damged(view->output, &box, output_damage) || true) {
		if(view->mapped) {
			skia_draw_surface(view->output->skia, view->surface, box.x, box.y, box.width, box.height);
		}
	}
	// damage finish

	struct timespec repaint_time;

	clockid_t presentation_clock =
		wlr_backend_get_presentation_clock(view->output->compositor->wlr_backend);

	clock_gettime(presentation_clock, &repaint_time);

	wlr_presentation_surface_sampled_on_output(
		view->output->compositor->wlr_presentation, view->surface, view->output->wlr_output);

	struct sc_view *subview;
	wl_list_for_each_reverse(subview, &view->children, link) {
		sc_render_view(subview, x+view->frame.x, y+view->frame.y, output_damage);
	}
}

void
sc_render_output(struct sc_output *output, struct timespec *when,
				 pixman_region32_t *output_damage)
{
	struct wlr_output *wlr_output = output->wlr_output;
	struct wlr_renderer *renderer = output->compositor->wlr_renderer;

	wlr_renderer_begin(renderer, wlr_output->width, wlr_output->height);
	
	if (!pixman_region32_not_empty(output_damage)) {
		// Output isn't damaged but needs buffer swap
		goto renderer_end;
	}

	GLint currentFb = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFb);
	glBindFramebuffer(GL_FRAMEBUFFER, output->fbo->framebuffer);
	skia_draw(output->skia);

	struct sc_workspace *workspace = output->compositor->current_workspace;

	struct sc_toplevel_view *toplevel_view;
	wl_list_for_each_reverse (toplevel_view, &workspace->views_toplevel, link) {
		sc_render_view(&toplevel_view->super, 0, 0, output_damage);
	}

	skia_submit(output->skia);

 	glBindFramebuffer(GL_FRAMEBUFFER, currentFb);


	struct wlr_gles2_texture_attribs *tex_attribs =
			malloc(sizeof(struct wlr_gles2_texture_attribs));
	tex_attribs->target = GL_TEXTURE_2D;
	tex_attribs->has_alpha = true;
	tex_attribs->tex = output->fbo->tex;

	sc_render_texture_with_output(
		tex_attribs, 0, 0, output->fbo->width,
		output->fbo->height, WL_OUTPUT_TRANSFORM_FLIPPED_180, output);

	free(tex_attribs);
renderer_end:
	
	
	wlr_renderer_scissor(renderer, NULL);
	wlr_output_render_software_cursors(wlr_output, output_damage);
	wlr_renderer_end(renderer);


	int width, height;
	wlr_output_transformed_resolution(wlr_output, &width, &height);

	pixman_region32_t frame_damage;
	pixman_region32_init(&frame_damage);

	enum wl_output_transform transform =
		wlr_output_transform_invert(wlr_output->transform);
	wlr_region_transform(&frame_damage, &output->damage->current, transform,
						 width, height);

	wlr_output_set_damage(wlr_output, &frame_damage);
	pixman_region32_fini(&frame_damage);

	if (!wlr_output_commit(wlr_output)) {
		return;
	}
	output->last_frame = *when;
}
