#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include "log.h"
#include "sc_fbo.h"

// #include "gles2_renderer.h"
// #include <unistd.h>
void gl_begin();

struct sc_fbo *
fbo_create(int w, int h)
{
	gl_begin();
	struct sc_fbo *fbo = (struct sc_fbo *) calloc(1, sizeof(struct sc_fbo));

	LOG("fbo_create %dx%d\n", w, h);
	GLuint tex;
    GLuint framebuffer;

	glGenFramebuffers(1, &framebuffer);
    LOG("%u\n", framebuffer);
    fbo->framebuffer = framebuffer;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo->framebuffer);
	glGenTextures(1, &tex);
	fbo->tex = tex;
	fbo->width = w;
	fbo->height = h;
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				 NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   tex, 0);

    GLenum fbstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbstatus == GL_FRAMEBUFFER_COMPLETE) {
		LOG("gen fbo good! %d\n", fbo->framebuffer);
	} else {
		LOG("fbo went wrong %d\n", fbstatus);
        if(fbstatus == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
            LOG("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
        }
		if(fbstatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
            LOG("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
        }
		if(fbstatus == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) {
            LOG("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");
        }
		if(fbstatus == GL_FRAMEBUFFER_UNSUPPORTED) {
            LOG("GL_FRAMEBUFFER_UNSUPPORTED\n");
        }
        exit(1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}
