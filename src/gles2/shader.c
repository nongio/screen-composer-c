#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"
#include "sc_shader.h"
#include "utils.h"
#include "sc_config.h"

extern struct sc_configuration configuration;

char *asprintf(const char *fmt, ...)
{

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	size_t needed = vsnprintf(NULL, 0, fmt, arg_ptr) +1;
    char *buffer = malloc(needed);
	va_end(arg_ptr);
	va_start(arg_ptr, fmt);
    vsnprintf(buffer, needed, fmt, arg_ptr);
	va_end(arg_ptr);
	return buffer;
}

GLuint shader_compile(const char *src, GLuint type)
{
  GLuint shader = glCreateShader(type);
  GLint	 ok;
  if (shader == 0)
    {
      DLOG("unable to create shader\n");
      goto shadererror;
    }
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (ok == GL_FALSE)
    {
    shadererror:
      DLOG("error compiling\n");
      DLOG("source:\n%s", src);

      GLsizei log_length = 0;
      GLchar  message[1024];
      glGetShaderInfoLog(shader, 1024, &log_length, message);

      DLOG("error:\n%s\n", message);
      glDeleteShader(shader);
      shader = 0;
    }

  return shader;
}

void
shader_link(struct sc_shader *shader, const char *vert_src,
			const char *frag_src)
{
  shader->_vert = shader_compile(vert_src, GL_VERTEX_SHADER);
  if (!shader->_vert)
    {
      DLOG("error compile vert\n");
      goto error;
    }

  shader->_frag = shader_compile(frag_src, GL_FRAGMENT_SHADER);
  if (!shader->_frag)
    {
      DLOG("error compile frag\n");
      glDeleteShader(shader->_vert);
      goto error;
    }

  shader->program = glCreateProgram();
  glAttachShader(shader->program, shader->_vert);
  glAttachShader(shader->program, shader->_frag);
  glLinkProgram(shader->program);

  glDetachShader(shader->program, shader->_vert);
  glDetachShader(shader->program, shader->_frag);
  glDeleteShader(shader->_vert);
  glDeleteShader(shader->_frag);

  GLint ok;
  glGetProgramiv(shader->program, GL_LINK_STATUS, &ok);
  if (ok == GL_FALSE)
    {
      glDeleteProgram(shader->program);
      goto error;
    }
  DLOG("program linked succesfully\n");
  return;

error:
  DLOG("problem linking program\n");
  return;
}

void shader_setup_default_uniforms(struct sc_shader *shader)
{
  shader->proj = glGetUniformLocation(shader->program, "proj");
  shader->color = glGetUniformLocation(shader->program, "color");
  shader->alpha = glGetUniformLocation(shader->program, "alpha");
  shader->invert_y = glGetUniformLocation(shader->program, "invert_y");
  shader->texsize = glGetUniformLocation(shader->program, "texsize");
  shader->texpos = glGetUniformLocation(shader->program, "texpos");
  shader->tex = glGetUniformLocation(shader->program, "tex");
  shader->pos_attrib = glGetAttribLocation(shader->program, "pos");
  shader->tex_attrib = glGetAttribLocation(shader->program, "texcoord");

}

struct sc_shader *
sc_shader_create(const char *name)
{
	DLOG("sc_shader_create: %s\n", name);
	struct sc_shader *shader = calloc(1, sizeof(struct sc_shader));
	char *fragname = asprintf("%s/%s.frag", configuration.shaders_path, name);
	const char *fragment_src = sc_read_file(fragname);
	char *vertname = asprintf("%s/%s.vert", configuration.shaders_path, name);
	const char *vertex_src = sc_read_file(vertname);
	shader_link(shader, vertex_src, fragment_src);
	shader_setup_default_uniforms(shader);

	free((void *)fragname);
	free((void *)vertname);
	free((void *)fragment_src);
	free((void *)vertex_src);
	return shader;
}

void sc_shader_begin(struct sc_shader *shader)
{
  glUseProgram(shader->program);
}
