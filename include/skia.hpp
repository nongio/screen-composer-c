#ifndef _SC_SKIA_H
#define _SC_SKIA_H

#define SK_GL

#include <include/gpu/GrDirectContext.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <include/core/SkSurface.h>

struct skia_context {
    sk_sp<GrDirectContext> context;
    sk_sp<SkSurface> surface;
};

struct skia_image {
    struct sc_texture_attributes *texture;
    sk_sp<SkImage> img;
};


#endif