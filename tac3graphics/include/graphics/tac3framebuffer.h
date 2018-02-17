#pragma once

#include "graphics\tac3gl.h"
#include <string>
#include <map>
#include <vector>


namespace Tac
{
  class Texture;

  class Framebuffer
  {
  public:
    static Framebuffer * GetBackbuffer(); 
    Framebuffer(const std::string & name);
    ~Framebuffer();

    bool HasTexture(GLuint attachmentPoint) const;
    Texture * GetTexture(GLuint attachmentPoint)const;
    void AttachTexture(Texture *, GLuint attachmentPoint);
    void DetachTexture(GLuint attachmentPoint);
    GLuint GetHandle() const;
    bool IsComplete();

    //GLsizei GetBufCount();
    //GLenum * GetBufs();
    //void CallglDrawBuffers(); /// todo: refactor
    //void Bind(const std::vector<GLenum> & bufs);
    std::vector<GLenum> GetColorAttachmentPoints();

    unsigned GetNumTextures()const;

  private:
    std::string mName;
    GLuint mHandle;
    std::vector<GLenum> mBufs;
    Framebuffer(); // used to create backbuffer

    // not owned
    std::map<GLuint, Texture*> mAttachedTextures;

  };
}
