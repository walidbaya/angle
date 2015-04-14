//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// FramebufferAttachment.h: Defines the wrapper class gl::FramebufferAttachment, as well as the
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4.3 page 108.

#ifndef LIBANGLE_FRAMEBUFFERATTACHMENT_H_
#define LIBANGLE_FRAMEBUFFERATTACHMENT_H_

#include "angle_gl.h"
#include "common/angleutils.h"
#include "libANGLE/ImageIndex.h"
#include "libANGLE/RefCountObject.h"

namespace egl
{
class Surface;
}

namespace gl
{
class FramebufferAttachmentObject;
class Renderbuffer;
class Texture;

// FramebufferAttachment implements a GL framebuffer attachment.
// Attachments are "light" containers, which store pointers to ref-counted GL objects.
// We support GL texture (2D/3D/Cube/2D array) and renderbuffer object attachments.
// Note: Our old naming scheme used the term "Renderbuffer" for both GL renderbuffers and for
// framebuffer attachments, which confused their usage.

class FramebufferAttachment : angle::NonCopyable
{
  public:
    FramebufferAttachment(GLenum binding,
                          const ImageIndex &textureIndex,
                          FramebufferAttachmentObject *resource);
    virtual ~FramebufferAttachment();

    // A framebuffer attachment points to one of three types of resources: Renderbuffers,
    // Textures and egl::Surface. The "Target" struct indicates which part of the
    // object an attachment references. For the three types:
    //   - a Renderbuffer has a unique renderable target, and needs no target index
    //   - a Texture has targets for every image and uses an ImageIndex
    //   - a Surface has targets for Color and Depth/Stencil, and uses the attachment binding
    class Target
    {
      public:
        Target(GLenum binding, const ImageIndex &imageIndex);
        Target(const Target &other);
        Target &operator=(const Target &other);

        GLenum binding() const { return mBinding; }
        const ImageIndex &textureIndex() const { return mTextureIndex; }

      private:
        GLenum mBinding;
        ImageIndex mTextureIndex;
    };

    // Helper methods
    GLuint getRedSize() const;
    GLuint getGreenSize() const;
    GLuint getBlueSize() const;
    GLuint getAlphaSize() const;
    GLuint getDepthSize() const;
    GLuint getStencilSize() const;
    GLenum getComponentType() const;
    GLenum getColorEncoding() const;

    bool isTextureWithId(GLuint textureId) const { return type() == GL_TEXTURE && id() == textureId; }
    bool isRenderbufferWithId(GLuint renderbufferId) const { return type() == GL_RENDERBUFFER && id() == renderbufferId; }

    GLenum getBinding() const { return mTarget.binding(); }
    GLuint id() const { return mResource.id(); }

    // These methods are only legal to call on Texture attachments
    const ImageIndex &getTextureImageIndex() const;
    GLenum cubeMapFace() const;
    GLint mipLevel() const;
    GLint layer() const;

    GLsizei getWidth() const;
    GLsizei getHeight() const;
    GLenum getInternalFormat() const;
    GLsizei getSamples() const;

    // Child class interface
    virtual GLenum type() const = 0;

    virtual Texture *getTexture() const = 0;
    virtual Renderbuffer *getRenderbuffer() const = 0;

  protected:
    Target mTarget;
    BindingPointer<FramebufferAttachmentObject> mResource;
};

class TextureAttachment : public FramebufferAttachment
{
  public:
    TextureAttachment(GLenum binding, Texture *texture, const ImageIndex &index);
    virtual ~TextureAttachment();

    virtual GLenum type() const;

    virtual Renderbuffer *getRenderbuffer() const;

    Texture *getTexture() const override;
};

class RenderbufferAttachment : public FramebufferAttachment
{
  public:
    RenderbufferAttachment(GLenum binding, Renderbuffer *renderbuffer);

    virtual ~RenderbufferAttachment();

    virtual GLenum type() const;

    virtual Texture *getTexture() const;

    Renderbuffer *getRenderbuffer() const override;
};

class DefaultAttachment : public FramebufferAttachment
{
  public:
    DefaultAttachment(GLenum binding, egl::Surface *surface);

    virtual ~DefaultAttachment();

    virtual GLenum type() const;

    virtual Texture *getTexture() const;
    virtual Renderbuffer *getRenderbuffer() const;

    const egl::Surface *getSurface() const;
};

// A base class for objects that FBO Attachments may point to.
class FramebufferAttachmentObject : public RefCountObject
{
  public:
    FramebufferAttachmentObject(GLuint id) : RefCountObject(id) {}

    virtual GLsizei getAttachmentWidth(const FramebufferAttachment::Target &target) const = 0;
    virtual GLsizei getAttachmentHeight(const FramebufferAttachment::Target &target) const = 0;
    virtual GLenum getAttachmentInternalFormat(const FramebufferAttachment::Target &target) const = 0;
    virtual GLsizei getAttachmentSamples(const FramebufferAttachment::Target &target) const = 0;
};

}

#endif // LIBANGLE_FRAMEBUFFERATTACHMENT_H_
