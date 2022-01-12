#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <wlr/render/gles2.h>
#include <wlr/util/region.h>

#include "gles2_renderer.h"
#include "log.h"
#include "sc_compositor.h"
#include "sc_output.h"
#include "sc_toplevel_view.h"
#include "sc_view.h"
#include "sc_workspace.h"

struct render_data {
	struct sc_output *output;
	pixman_region32_t *damage;
	struct sc_view *view;
};

void sc_compositor_setup_gles2()
{
	sc_renderer_load_shaders();
}

void render_surface(struct wlr_surface *surface, int x, int y,
		void * data)
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

		sc_render_texture_with_output(tex_attribs, sx + x, sy + y, surface->current.width,
					   surface->current.height, surface->current.transform,
					   output);

		free(tex_attribs);
	}
	// damage finish
	pixman_region32_fini(&damage);

	struct timespec repaint_time;

	clockid_t presentation_clock =
		wlr_backend_get_presentation_clock(output->compositor->wlr_backend);

	clock_gettime(presentation_clock, &repaint_time);

	wlr_presentation_surface_sampled_on_output(output->compositor->wlr_presentation,
											   surface, output->wlr_output);
}

void sc_render_output(struct sc_output *output, struct timespec *when,
		pixman_region32_t *damage) {
	struct wlr_output *wlr_output = output->wlr_output;
	struct wlr_renderer *renderer = output->compositor->wlr_renderer;

	wlr_renderer_begin(renderer, wlr_output->width, wlr_output->height);

	if (!pixman_region32_not_empty(damage)) {
		// Output isn't damaged but needs buffer swap
		goto renderer_end;
	}

	float clear_color[] = {0.25f, 0.25f, 0.25f, 1.0f};
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT);

	struct render_data render_data = {
		.output = output,
		.damage = damage,
	};

	struct sc_toplevel_view *toplevel_view;
	struct sc_workspace *workspace = output->compositor->current_workspace;
	wl_list_for_each_reverse(toplevel_view, &workspace->views_toplevel, link) {

		render_data.view = &toplevel_view->super;
		sc_view_for_each_surface(&toplevel_view->super, render_surface, &render_data);
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
	wlr_region_transform(&frame_damage, &output->damage->current,
		transform, width, height);

	wlr_output_set_damage(wlr_output, &frame_damage);
	pixman_region32_fini(&frame_damage);

	if (!wlr_output_commit(wlr_output)) {
		return;
	}
	output->last_frame = *when;
}


