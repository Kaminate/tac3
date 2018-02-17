/*!
\author Nathan Park
*/
#pragma once

#include "graphics\tac3gl.h"
#include "freeimage\FreeImage.h"
#include <string>

namespace Tac
{
  class Image
  {
  public:
    Image(const Image & rhs);
    Image & operator =(const Image & rhs);
    Image();
    ~Image();
    void Clear();
    std::string Load(const std::string & filepath);
    std::string Save( const std::string & mNormalMapSavepath );
    FIBITMAP * mBitmap;
    int mWidth;
    int mHeight;
    int mPitch;
    int mBitsPerPixel;
    BYTE * mBytes;
  };



  class Texture
  {

  public:
    struct Properties
    {
      unsigned mWidth;
      unsigned mHeight;
      unsigned mDepth; // only used in 3d textures.......
      GLuint mTarget; // ie: GL_TEXTURE_2D
      GLuint mCPUPixelFormat; // ex: GL_BGRA, GL_RGB, GL_RED, GL_DEPTH_COMPONENT
      GLuint mCPUPixelType; // ex: GL_FLOAT
      GLuint mGPUInternalFormat; // ex: GL_RGBA8
      // numMips?
    };

    Texture();
    ~Texture();
    void Clear();
    std::string Load(
      const Properties & props,
      void * pixelData );
  // example use: std::string errors = Load("file.png", gl_rgba8)
    std::string Load( 
      const std::string & filename,
      GLint internalFormat );
    std::string Load( const Image & myImage, GLint internalFormat );

    GLuint GetHandle() const;
    const Properties &GetProperties()const;
    void SetName(const std::string & name);
    const std::string & GetName()const;
    void Reload();
  private:

    std::string mFilepath;
    std::string mName;
    GLuint mHandle;
    Properties mProperties;
  };

} // Tac

