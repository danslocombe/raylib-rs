#include "pbo_readback.h"

#include <stdlib.h>
#include <string.h>

// glad declarations only -- we deliberately do NOT define GLAD_GL_IMPLEMENTATION
// here. That gives us the GL enum defines, the function-pointer typedefs, and the
// `#define glReadPixels glad_glReadPixels` aliases, while the pointers themselves
// are defined (and loaded at GL init) inside rlgl's translation unit, which this
// object links against. glad.h is self-contained (it bundles khrplatform).
#include "../raylib/src/external/glad.h"

struct PboReadback
{
    unsigned int fbo;
    unsigned int pbo[2];
    int width;
    int height;
    int write_idx; // PBO index we glReadPixels into this frame
    int primed;    // 0 until at least one read has been issued
};

PboReadback *pbo_readback_create(int width, int height)
{
    if (width <= 0 || height <= 0)
        return NULL;

    PboReadback *rb = (PboReadback *)calloc(1, sizeof(PboReadback));
    if (!rb)
        return NULL;

    rb->width = width;
    rb->height = height;
    rb->write_idx = 0;
    rb->primed = 0;

    glGenFramebuffers(1, &rb->fbo);
    glGenBuffers(2, rb->pbo);

    const GLsizeiptr bytes = (GLsizeiptr)width * (GLsizeiptr)height * 4;
    for (int i = 0; i < 2; i++)
    {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, rb->pbo[i]);
        glBufferData(GL_PIXEL_PACK_BUFFER, bytes, NULL, GL_STREAM_READ);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    return rb;
}

void pbo_readback_destroy(PboReadback *rb)
{
    if (!rb)
        return;
    if (rb->pbo[0] || rb->pbo[1])
        glDeleteBuffers(2, rb->pbo);
    if (rb->fbo)
        glDeleteFramebuffers(1, &rb->fbo);
    free(rb);
}

int pbo_readback_grab(PboReadback *rb, unsigned int tex_id, unsigned char *out)
{
    if (!rb || !out)
        return 0;

    // Snapshot the bindings we touch so raylib's GL state is left untouched.
    GLint prev_read_fbo = 0;
    GLint prev_pack_pbo = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prev_read_fbo);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &prev_pack_pbo);

    const int read_idx = rb->write_idx ^ 1;

    // Bind our FBO as the read source and attach the (ping-pong) texture. The
    // texture changes between frames, so we re-attach every call.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, rb->fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    // Kick this frame's async read. With a PBO bound to GL_PIXEL_PACK_BUFFER the
    // final arg is a byte offset, not a client pointer, and the transfer is queued
    // on the GPU rather than blocking the CPU.
    glBindBuffer(GL_PIXEL_PACK_BUFFER, rb->pbo[rb->write_idx]);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, rb->width, rb->height, GL_RGBA, GL_UNSIGNED_BYTE, (void *)0);

    int filled = 0;
    if (rb->primed)
    {
        // Map last frame's PBO; that read has had a whole frame to complete, so
        // the map does not stall.
        glBindBuffer(GL_PIXEL_PACK_BUFFER, rb->pbo[read_idx]);
        void *mapped = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
        if (mapped)
        {
            memcpy(out, mapped, (size_t)rb->width * (size_t)rb->height * 4);
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            filled = 1;
        }
    }

    rb->write_idx = read_idx;
    rb->primed = 1;

    // Restore the bindings we changed.
    glBindBuffer(GL_PIXEL_PACK_BUFFER, (unsigned int)prev_pack_pbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, (unsigned int)prev_read_fbo);

    return filled;
}
