#ifndef _SC_FBO_H
#define _SC_FBO_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

struct sc_fbo {
	int width;
	int height;
    GLuint framebuffer;
	GLuint tex;
};

struct sc_fbo *
fbo_create(int w, int h);
#endif

