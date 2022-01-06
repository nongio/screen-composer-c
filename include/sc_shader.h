#ifndef _SC_SHADER_H
#define _SC_SHADER_H
#include <GLES2/gl2.h>

struct sc_shader {
  GLuint proj;
  GLuint invert_y;
  GLuint tex;
  GLuint alpha;
  GLuint color;
  GLuint texsize;
  GLuint texpos;
  GLuint pos_attrib;
  GLuint tex_attrib;
  GLuint program;

  GLuint _vert;
  GLuint _frag;
};

struct sc_shader *sc_shader_create(const char *name);
void sc_shader_begin();
#endif

