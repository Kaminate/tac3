// Nathan Park
#include "graphics\tac3framebuffer.h"
#include "graphics\tac3texture.h"
#include <assert.h>
namespace Tac
{
  Framebuffer * Framebuffer::GetBackbuffer()
  {
    static Framebuffer * backbuffer = new Framebuffer();
    return backbuffer;
  }
  GLuint Framebuffer::GetHandle() const
  {
    return mHandle;
  }
  Framebuffer::Framebuffer(const std::string & name) 
    : mName(name)
  {
    glGenFramebuffers(1, &mHandle);
  }
  Framebuffer::Framebuffer()
    : mHandle( 0 )
  {
  }
  Framebuffer::~Framebuffer()
  {
    glDeleteFramebuffers(1, &mHandle);
  }
  void Framebuffer::AttachTexture(
    Texture* tex,
    GLuint attachmentPoint )
  {
#ifdef _DEBUG
    if(!tex)
    {
      // call DetachTexture() instead
      __debugbreak();
    }
    if( !tex->GetHandle() )
    {
      // the texture wasnt loaded!
      __debugbreak();
    }

    if(mAttachedTextures[attachmentPoint]) 
      __debugbreak(); // already bound
#else
    assert(tex);
    assert(tex->GetHandle());
    assert(!mAttachedTextures[attachmentPoint]);
#endif

    mAttachedTextures[attachmentPoint] = tex;
    glBindFramebuffer(GL_FRAMEBUFFER, mHandle);

    glFramebufferTexture(
      GL_FRAMEBUFFER, 
      attachmentPoint,
      tex->GetHandle(), 
      0); // mipmap level

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  bool Framebuffer::IsComplete()
  {
    std::string error;

    glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_UNDEFINED:
      {
        error = "GL_FRAMEBUFFER_UNDEFINED is returned if target is the default framebuffer, but the default framebuffer does not exist.";
      } break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      {
        error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT is returned if any of the framebuffer attachment points are framebuffer incomplete.";
      } break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
      {
        error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT is returned if the framebuffer does not have at least one image attached to it.";
      } break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      {
        error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER is returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi.";
      } break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      {
        error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER is returned if GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.";
      } break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
      {
        error = " GL_FRAMEBUFFER_UNSUPPORTED is returned if the combination of internal formats of the attached images violates an implementation-dependent set of restrictions.";
      } break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE :
      {
        error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE is returned if the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES."
          "\n"
          "\n"
          "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE is also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.";

      }break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
      {

        error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS is returned if any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target.";
      } break;

    case GL_FRAMEBUFFER_COMPLETE:
      // do nothing, error stays empty
      break;

    case 0:
      error = "GL_INVALID_ENUM is generated if target is not GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER.";
      break;

    default:
      error = "???";
      break;
    }

    OutputDebugString( (LPCSTR)error.c_str() );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#if _DEBUG
    if( !error.empty())
      __debugbreak();
#endif
    return error.empty();
  }
  Texture * Framebuffer::GetTexture( GLuint attachmentPoint )const
  {
    return mAttachedTextures.at(attachmentPoint);
  }
  bool Framebuffer::HasTexture( GLuint attachmentPoint ) const
  {
    return mAttachedTextures.find(attachmentPoint) !=
      mAttachedTextures.end();
  }
  unsigned Framebuffer::GetNumTextures()const
  {
    return mAttachedTextures.size();
  }
  void Framebuffer::DetachTexture( GLuint attachmentPoint )
  {
#ifdef _DEBUG
    if(!mAttachedTextures[attachmentPoint]) 
      __debugbreak(); // no texture at this attachment point
#else
    assert(mAttachedTextures[attachmentPoint]);
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
    glFramebufferTexture(
      GL_FRAMEBUFFER, 
      attachmentPoint,
      0,
      0); // mipmap level
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mAttachedTextures.erase(attachmentPoint);

  }
  std::vector<GLenum> Framebuffer::GetColorAttachmentPoints()
  {
    std::vector<GLenum> colorAttachmentPoints;

    for(const std::pair<GLuint, Texture*>& curPair : mAttachedTextures)
    {
      GLuint curAttachPt = curPair.first;
      if(curAttachPt != GL_DEPTH_ATTACHMENT)
      {
        colorAttachmentPoints.push_back(curAttachPt);
      }
    }

    return colorAttachmentPoints;
  }
}