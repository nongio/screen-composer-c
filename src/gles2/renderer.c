#define _POSIX_C_SOURCE 200809L
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include <wlr/render/egl.h>
#include <wlr/render/gles2.h>
#include <wlr/types/wlr_matrix.h>

#include "log.h"
#include "sc_output.h"
#include "sc_shader.h"

static const GLfloat texcoord[] = {
	1, 0, // top right
	0, 0, // top left
	1, 1, // bottom right
	0, 1, // bottom left
};

static const GLfloat verts[] = {
	1, 0, // top right
	0, 0, // top left
	1, 1, // bottom right
	0, 1, // bottom left
};

static const float flip_180[9] = {
	1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};


static GLuint vbo_vert;
static GLuint vbo_texcoord;

static struct sc_shader *shader_texture_rgba;
static struct sc_shader *shader_texture_rgbx;
static struct sc_shader *shader_texture_external;

extern struct sc_compositor *compositor;

void
gl_begin()
{
	if (!wlr_egl_is_current(compositor->egl)) {
		wlr_egl_make_current(compositor->egl);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void
sc_renderer_load_shaders()
{
	gl_begin();
	shader_texture_rgba = sc_shader_create("textureRGBA");
	shader_texture_rgbx = sc_shader_create("textureRGBX");
	shader_texture_external = sc_shader_create("textureExternal");

	/* Load vertex buffers for the shaders */

	GLuint buffers[2];
	glGenBuffers(2, buffers);

	vbo_vert = buffers[0];
	vbo_texcoord = buffers[1];

	const int VERTEX_BUFFER_SIZE = 8;
	const int VERTEX_BUFFER_BYTESIZE = sizeof(GLfloat) * VERTEX_BUFFER_SIZE;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vert);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_BYTESIZE, verts,
				 GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_BYTESIZE, texcoord,
				 GL_STATIC_DRAW);
}

struct sc_shader *
shader_for_texattribs(struct wlr_gles2_texture_attribs *attribs)
{
  struct sc_shader *shader;
  switch (attribs->target)
    {
    case GL_TEXTURE_2D:
      if (attribs->has_alpha)
	{
	  shader = shader_texture_rgba;
	}
      else
	{
	  shader = shader_texture_rgbx;
	}
      break;
    case GL_TEXTURE_EXTERNAL_OES:
      shader = shader_texture_external;
      break;
    default:
      ELOG("error: can't find shader for texture attributes...\n");
      shader = shader_texture_rgba;
    }

  return shader;
}

void
render_texture_with_matrix(struct wlr_gles2_texture_attribs *texture, int sx,
						   int sy, int w, int h, float *matrix)
{
	struct sc_shader *shader = shader_for_texattribs(texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture->target, texture->tex);
	glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0 + 1);

	sc_shader_begin(shader);
	// uniformsForShader(shader);

	glUniformMatrix3fv(shader->proj, 1, GL_FALSE, matrix);
	//  glUniform1i(shader->invert_y, texture->inverted_y);
	glUniform1i(shader->tex, 0);
	glUniform1f(shader->alpha, 1.0);
	glUniform3f(shader->texsize, w, h, 0);
	glUniform3f(shader->texpos, sx, sy, 0);

	glEnableVertexAttribArray(shader->pos_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vert);
	glVertexAttribPointer(shader->pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(shader->tex_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
	glVertexAttribPointer(shader->tex_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// glDisableVertexAttribArray(shader->pos_attrib);
	// glDisableVertexAttribArray(shader->tex_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
sc_render_texture_with_output(struct wlr_gles2_texture_attribs *texture, int sx,
							  int sy, int w, int h, enum wl_output_transform t,
							  struct sc_output *output)
{

	double ox = output->output_box->x;
	double oy = output->output_box->y;

//  struct wlr_output *o = output->wlr_output;
//	wlr_output_layout_output_coords(output->layout, o, &ox, &oy);

	ox += sx;
	oy += sy;

	struct wlr_box box = {
		.x = sx,
		.y = sy,
		.width = w,
		.height = h,
	};

	float output_transform_matrix[9];
	wlr_matrix_identity(output_transform_matrix);

	float gl_matrix[9];
	enum wl_output_transform transform = wlr_output_transform_invert(t);
	wlr_matrix_project_box(gl_matrix, &box, transform, 0, output->projection_matrix);


	wlr_matrix_multiply(gl_matrix, flip_180, gl_matrix);

	wlr_matrix_transpose(gl_matrix, gl_matrix);

	render_texture_with_matrix(texture, sx, sy, w, h, gl_matrix);
}
