//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// FramebufferAttachment.cpp: the gl::FramebufferAttachment class and its derived classes
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4.3 page 108.

#include "libANGLE/FramebufferAttachment.h"

#include "common/utilities.h"
#include "libANGLE/Config.h"
#include "libANGLE/Renderbuffer.h"
#include "libANGLE/Surface.h"
#include "libANGLE/Texture.h"
#include "libANGLE/formatutils.h"
#include "libANGLE/renderer/FramebufferImpl.h"

namespace gl
{

////// FramebufferAttachment::Target Implementation //////

FramebufferAttachment::Target::Target(GLenum binding, const ImageIndex &imageIndex)
    : mBinding(binding),
      mTextureIndex(imageIndex)
{
}

FramebufferAttachment::Target::Target(const Target &other)
    : mBinding(other.mBinding),
      mTextureIndex(other.mTextureIndex)
{
}

FramebufferAttachment::Target &FramebufferAttachment::Target::operator=(const Target &other)
{
    this->mBinding = other.mBinding;
    this->mTextureIndex = other.mTextureIndex;
    return *this;
}

////// FramebufferAttachment Implementation //////

FramebufferAttachment::FramebufferAttachment(GLenum binding,
                                             const ImageIndex &textureIndex,
                                             FramebufferAttachmentObject *resource)
    : mTarget(binding, textureIndex)
{
    mResource.set(resource);
}

FramebufferAttachment::~FramebufferAttachment()
{
    mResource.set(nullptr);
}

GLuint FramebufferAttachment::getRedSize() const
{
    return GetInternalFormatInfo(getInternalFormat()).redBits;
}

GLuint FramebufferAttachment::getGreenSize() const
{
    return GetInternalFormatInfo(getInternalFormat()).greenBits;
}

GLuint FramebufferAttachment::getBlueSize() const
{
    return GetInternalFormatInfo(getInternalFormat()).blueBits;
}

GLuint FramebufferAttachment::getAlphaSize() const
{
    return GetInternalFormatInfo(getInternalFormat()).alphaBits;
}

GLuint FramebufferAttachment::getDepthSize() const
{
    return GetInternalFormatInfo(getInternalFormat()).depthBits;
}

GLuint FramebufferAttachment::getStencilSize() const
{
    return GetInternalFormatInfo(getInternalFormat()).stencilBits;
}

GLenum FramebufferAttachment::getComponentType() const
{
    return GetInternalFormatInfo(getInternalFormat()).componentType;
}

GLenum FramebufferAttachment::getColorEncoding() const
{
    return GetInternalFormatInfo(getInternalFormat()).colorEncoding;
}

const ImageIndex &FramebufferAttachment::getTextureImageIndex() const
{
    ASSERT(type() == GL_TEXTURE);
    return mTarget.textureIndex();
}

GLenum FramebufferAttachment::cubeMapFace() const
{
    ASSERT(type() == GL_TEXTURE);

    const auto &index = mTarget.textureIndex();
    return IsCubeMapTextureTarget(index.type) ? index.type : GL_NONE;
}

GLint FramebufferAttachment::mipLevel() const
{
    ASSERT(type() == GL_TEXTURE);
    return mTarget.textureIndex().mipIndex;
}

GLint FramebufferAttachment::layer() const
{
    ASSERT(type() == GL_TEXTURE);

    const auto &index = mTarget.textureIndex();

    if (index.type == GL_TEXTURE_2D_ARRAY || index.type == GL_TEXTURE_3D)
    {
        return index.layerIndex;
    }
    return 0;
}

GLsizei FramebufferAttachment::getWidth() const
{
    return mResource->getAttachmentWidth(mTarget);
}

GLsizei FramebufferAttachment::getHeight() const
{
    return mResource->getAttachmentHeight(mTarget);
}

GLenum FramebufferAttachment::getInternalFormat() const
{
    return mResource->getAttachmentInternalFormat(mTarget);
}

GLsizei FramebufferAttachment::getSamples() const
{
    return mResource->getAttachmentSamples(mTarget);
}

///// TextureAttachment Implementation ////////

TextureAttachment::TextureAttachment(GLenum binding, Texture *texture, const ImageIndex &index)
    : FramebufferAttachment(binding, index, texture)
{
}

TextureAttachment::~TextureAttachment()
{
}

GLenum TextureAttachment::type() const
{
    return GL_TEXTURE;
}

Renderbuffer *TextureAttachment::getRenderbuffer() const
{
    UNREACHABLE();
    return nullptr;
}

Texture *TextureAttachment::getTexture() const
{
    return rx::GetAs<Texture>(mResource.get());
}

////// RenderbufferAttachment Implementation //////

RenderbufferAttachment::RenderbufferAttachment(GLenum binding, Renderbuffer *renderbuffer)
    : FramebufferAttachment(binding, ImageIndex::MakeInvalid(), renderbuffer)
{
    ASSERT(renderbuffer);
}

RenderbufferAttachment::~RenderbufferAttachment()
{
}

GLenum RenderbufferAttachment::type() const
{
    return GL_RENDERBUFFER;
}

Texture *RenderbufferAttachment::getTexture() const
{
    UNREACHABLE();
    return nullptr;
}

Renderbuffer *RenderbufferAttachment::getRenderbuffer() const
{
    return rx::GetAs<Renderbuffer>(mResource.get());
}

DefaultAttachment::DefaultAttachment(GLenum binding, egl::Surface *surface)
    : FramebufferAttachment(binding, ImageIndex::MakeInvalid(), surface)
{
}

DefaultAttachment::~DefaultAttachment()
{
}

GLenum DefaultAttachment::type() const
{
    return GL_FRAMEBUFFER_DEFAULT;
}

Texture *DefaultAttachment::getTexture() const
{
    UNREACHABLE();
    return nullptr;
}

Renderbuffer *DefaultAttachment::getRenderbuffer() const
{
    UNREACHABLE();
    return nullptr;
}

const egl::Surface *DefaultAttachment::getSurface() const
{
    return rx::GetAs<egl::Surface>(mResource.get());
}

}
