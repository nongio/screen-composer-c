#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglplatform.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "skia.hpp"
#include <map>
#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkRRect.h>
#include <include/core/SkRegion.h>
#include <include/core/SkBlurTypes.h>
#include <include/effects/SkImageFilters.h>
#include <include/core/SkTextBlob.h>

#include <include/effects/SkBlurMaskFilter.h>
#include <include/gpu/GrBackendSurface.h>
#include <include/gpu/gl/GrGLTypes.h>

extern "C" {
#include "log.h"
#include "sc_fbo.h"
#include <wlr/util/log.h>
#include "sc_skia.h"
#include "sc-layer-shell.h"
}

std::map<struct wlr_surface *, struct skia_image *> skia_images_cache;
sk_sp<SkImage> bg_img;

void load_bg() {
    sk_sp<SkData> img_data = SkData::MakeFromFileName("/home/riccardo/Pictures/image_processing20200924-9111-1wjd9k7.png");
    bg_img = SkImage::MakeFromEncoded(img_data);
}

extern "C" struct skia_context *skia_context_create_for_view(struct sc_fbo *fbo)
{
    struct skia_context *skia = (struct skia_context *)calloc(1, sizeof(struct skia_context));

    auto gl = GrGLMakeNativeInterface();
    gl->ref();
    skia->context = GrDirectContext::MakeGL(gl);
    skia->context->ref();

    SkImageInfo info = SkImageInfo:: MakeN32Premul(fbo->width * 1,
                                    fbo->height * 1);

    GrGLint buffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = fbo->framebuffer;
    fbInfo.fFormat = GL_RGBA8_OES;

    GrBackendRenderTarget backendRT(fbo->width * 1,
                                    fbo->height * 1,
                                    1,
                                    8,
                                    fbInfo);

    skia->surface = SkSurface::MakeFromBackendRenderTarget(skia->context.get(), backendRT,
                                                       kBottomLeft_GrSurfaceOrigin,
                                                       kRGBA_8888_SkColorType,
                                                       nullptr,
                                                       nullptr);
    skia->surface->ref();
    if (!skia->surface) {
        SkDebugf("SkSurface::MakeRenderTarget returned null\n");
        exit(1);
    }    
    
    return skia;
}

extern "C" void skia_draw(struct skia_context *skia) {

    skia->context->resetContext();
    if(bg_img == NULL) {
        load_bg();
    }
    SkCanvas *canvas = skia->surface->getCanvas();
    canvas->resetMatrix();
    
    canvas->clear(0xFF000000);
    canvas->drawImage(bg_img, 0, 0);

    SkPaint p;
	p.setStrokeWidth(1);
	p.setColor(0xFFfff6f6);
	p.setAntiAlias(true);
	p.setStyle(SkPaint::kFill_Style);

	SkPaint paint;
	SkFont font = SkFont();
    
	sk_sp<SkTypeface> tf = SkTypeface::MakeFromName("TeX Gyre Heros", SkFontStyle::Bold());
    //sk_sp<SkTypeface> emojitf = SkTypeface::MakeFromName("Emoji", SkFontStyle());
	;
	paint.setStyle(SkPaint::kFill_Style);
	paint.setColor(0xFF000000);

    font.setTypeface(tf);
    font.setSize(22);
    std::string text = "Screen Composer";
    sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromString(text.c_str(), font, SkTextEncoding::kUTF8);

    
    canvas->drawTextBlob(blob.get(), 10, 22, paint);
  
}

extern "C" void skia_submit(struct skia_context *skia) {
    skia->context->flushAndSubmit();
     glUseProgram(0);
     
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

extern "C" void free_skia_image(struct wlr_surface *surface) {
    struct skia_image * skia_image = skia_images_cache[surface];
    if(skia_image) {
        skia_image->img->unref();
        free(skia_image);
    }
    skia_images_cache.erase(surface);
}

extern "C" struct skia_image *skia_image_from_texture(struct skia_context *skia, struct wlr_surface *surface, struct sc_texture_attributes *texture_attributes)
{

    struct skia_image * skia_image = skia_images_cache[surface];
    if(skia_image) {
        skia_image->img->unref();
        free(skia_image);
    }
    skia_images_cache.erase(surface);

    GrGLTextureInfo tinfo = {
        .fTarget = texture_attributes->target,
        .fID = texture_attributes->tex,
        .fFormat = GL_RGBA8_OES,
    };

    GrBackendTexture dest = GrBackendTexture(texture_attributes->width,
                     texture_attributes->height,
                     GrMipMapped::kNo,
                     tinfo);

    sk_sp<SkImage> img = SkImage::MakeFromTexture(skia->context.get(), dest, kTopLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kOpaque_SkAlphaType, SkColorSpace::MakeSRGBLinear());
    skia_image = (struct skia_image *)calloc(1, sizeof(struct skia_image));

    img->ref();
    skia_image->img = img;
    skia_image->texture = texture_attributes;
    
    skia_images_cache[surface] = skia_image;

    return skia_image;
}

extern "C" void skia_draw_surface(struct skia_context *skia, struct wlr_surface *surface, int x, int y, int w, int h) {

    struct skia_image * skia_image = skia_images_cache[surface];

    if(skia_image != NULL) {
         SkCanvas *canvas = skia->surface->getCanvas();

       // SkMatrix matrix;
       // matrix.setScale(0.5f, 0.5f);
       // canvas->setMatrix(matrix);

        sk_sp<SkImage> image = skia_image->img;
        
        // int frame = 1;
        // sk_sp<SkImageFilter> shadowFilter = SkImageFilters::DropShadow(
                    // 0.0f, 0.0f, 10.0f, 10.0f, SK_ColorBLACK, nullptr);
        
        // sk_sp<SkImageFilter> offsetFilter = SkImageFilters::Offset(40, 40, shadowFilter, nullptr);
        // SkIRect subset = image->bounds();
        // SkIRect clipBounds = image->bounds();
        // clipBounds.outset(60, 60);
        // SkIRect outSubset;
        // SkIPoint offset;
        // sk_sp<SkImage> filtered(image->makeWithFilter(canvas->recordingContext(), offsetFilter.get(),
                                                    // subset, clipBounds, &outSubset, &offset));
        canvas->drawImage(image, x, y);

    }
}

extern "C" void skia_draw_layer(struct skia_context *skia, struct wlr_surface *surface, struct sc_layer_v1_state *layer){
    LOG("draw layer\n");
//    struct sc_view *view = layer->base;
    struct skia_image * skia_image = skia_images_cache[surface];

    if(skia_image != NULL) {
        SkCanvas *canvas = skia->surface->getCanvas();

        sk_sp<SkImage> image = skia_image->img;
        
                                                    // subset, clipBounds, &outSubset, &offset));
        SkRRect rrect = SkRRect::MakeRectXY({
            (float)layer->position.x,
            (float)layer->position.y,
            (float)layer->bounds.width,
            (float)layer->bounds.height
        },
            (float)layer->border_corner_radius,
            (float)layer->border_corner_radius);
        SkPaint p;
        p.setAntiAlias(true);
        p.setStyle(SkPaint::kFill_Style);
        p.setColor(SkColorSetARGB(
            layer->background_color.a,
            layer->background_color.r,
            layer->background_color.g,
            layer->background_color.b)
        );
        p.setAlphaf(layer->opacity);
        canvas->drawRRect(rrect, p);
        p.setStyle(SkPaint::kStroke_Style);
        p.setStrokeWidth((float)layer->border_width);
        p.setColor(SkColorSetARGB(
            layer->border_color.a,
            layer->border_color.r,
            layer->border_color.g,
            layer->border_color.b)
        );
        p.setAlphaf(layer->opacity);
        canvas->drawRRect(rrect, p);

        // draw the surface on top of the layer
        //canvas->drawImage(image, layer->position.x, layer->position.y);

    }
}


