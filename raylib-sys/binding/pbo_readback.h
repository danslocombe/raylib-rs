#ifndef PBO_READBACK_H
#define PBO_READBACK_H

#if defined(__cplusplus)
extern "C"
{ // Prevents name mangling of functions
#endif

    // Asynchronous GPU->CPU texture readback using a 2-deep pixel-buffer-object
    // (PBO) ring. Each frame we issue glReadPixels into one PBO (a non-blocking
    // DMA) and map the *other* PBO, which holds the read we kicked last frame and
    // has since completed. The CPU therefore never waits on the GPU: the returned
    // pixels are always one frame stale, which is invisible for the local-only,
    // non-deterministic flow-particle field they drive.
    //
    // Opaque handle: holds a persistent FBO + the two PBOs. Create once per source
    // texture (e.g. per gas world), on the GL thread, after the context + glad are
    // initialised.
    typedef struct PboReadback PboReadback;

    // Allocate a readback sized for a width*height RGBA8 texture. NULL on failure.
    PboReadback *pbo_readback_create(int width, int height);

    // Free the GL objects and the handle. Safe to call with NULL.
    void pbo_readback_destroy(PboReadback *rb);

    // Issue this frame's async read of `tex_id` into the write PBO, then copy the
    // previous frame's pixels into `out` (RGBA8, width*height*4 bytes, bottom-row
    // first to match glGetTexImage / LoadImageFromTexture, so existing sampling
    // orientation is preserved).
    //
    // Returns 1 if `out` was filled with a (>=1 frame old) field; returns 0 on the
    // very first call for this readback, when no prior frame exists yet -- in that
    // case `out` is left untouched and the caller should treat it as "no data".
    int pbo_readback_grab(PboReadback *rb, unsigned int tex_id, unsigned char *out);

#if defined(__cplusplus)
}
#endif

#endif // PBO_READBACK_H
