#include "graphics\tac3gl.h"
#include "graphics\tac3texture.h"
#include "tac3util\tac3utilities.h"
#include <memory>
#include <assert.h>
namespace Tac
{
  Texture::Texture() : mHandle(0), mName("Unnamed texture")
  {

  }
  Texture::~Texture()
  {
    Clear();
  }
  void Texture::Clear()
  {
    glDeleteTextures(1, &mHandle);
    mHandle = 0;
  }
  std::string Texture::Load( 
    const Properties & props,
    void * pixelData ) 
  {
    std::string errors;
    mProperties = props;
    glGenTextures(1, &mHandle);
    glBindTexture(props.mTarget, mHandle);
    switch (props.mTarget)
    {
    case GL_TEXTURE_1D: 
      glTexImage1D(
        props.mTarget, 0, props.mGPUInternalFormat,
        props.mWidth,
        0,
        props.mCPUPixelFormat, props.mCPUPixelType, pixelData);
      break;
    case GL_TEXTURE_2D: 
      glTexImage2D(
        props.mTarget, 0, props.mGPUInternalFormat,
        props.mWidth, props.mHeight,
        0,
        props.mCPUPixelFormat, props.mCPUPixelType, pixelData);
      break;
    case GL_TEXTURE_3D: 
      glTexImage3D(
        props.mTarget, 0, props.mGPUInternalFormat,
        props.mWidth, props.mHeight,props.mDepth,
        0,
        props.mCPUPixelFormat, props.mCPUPixelType, pixelData);
      break;
    }
    glTexParameterf(mProperties.mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(mProperties.mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(mProperties.mTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(mProperties.mTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(mProperties.mTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(mProperties.mTarget, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(props.mTarget, 0);
    return errors;
  }
  std::string Texture::Load(
    const std::string & filename,
    GLint internalFormat)
  {
    Image myImage;
    std::string errors = myImage.Load(filename);
    if(errors.empty())
    {
      errors = Load(myImage, internalFormat);
    }
    return errors;
  }
  std::string Texture::Load( 
    const Image & myImage,
    GLint internalFormat )
  {
    Clear();
    mProperties.mGPUInternalFormat = internalFormat;
    mProperties.mTarget = GL_TEXTURE_2D;
    mProperties.mDepth = 0;
    mProperties.mWidth = myImage.mWidth;
    mProperties.mHeight = myImage.mHeight;

    // NOTE( N8 ): During Image::Load, FreeImage converts to 32bit bgra
    assert( myImage.mBitsPerPixel == 32 );
    mProperties.mCPUPixelFormat = GL_BGRA;

    glGenTextures(1, &mHandle);
    glBindTexture(mProperties.mTarget, mHandle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(
      mProperties.mTarget,
      mProperties.mDepth,
      mProperties.mGPUInternalFormat, // actual format of how to store tex
      mProperties.mWidth,
      mProperties.mHeight,
      0,
      mProperties.mCPUPixelFormat,
      GL_UNSIGNED_BYTE, // pixel data format
      myImage.mBytes);

    glTexParameterf(mProperties.mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(mProperties.mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(mProperties.mTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(mProperties.mTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(mProperties.mTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(mProperties.mTarget, GL_TEXTURE_MAX_LEVEL, 0);

    glBindTexture(mProperties.mTarget, 0);
    return "";
  }
  GLuint Texture::GetHandle() const
  {
    return mHandle;
  }
  const Texture::Properties & Texture::GetProperties() const
  {
    return mProperties;
  }
  void Texture::SetName( const std::string & name )
  {
    mName = name;
  }
  const std::string & Texture::GetName() const
  {
    return mName;
  }
  void Texture::Reload()
  {
    __debugbreak();
  }

  static void FreeImageErrorHandler(
    FREE_IMAGE_FORMAT fif, const char *message) 
  {
    printf("\n*** "); 
    if(fif != FIF_UNKNOWN) {
      printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
    }
    printf(message);
    printf(" ***\n");
    __debugbreak();
  }
  std::string Image::Load( const std::string & filename )
  {
    Clear();
    static bool once = [](){
      FreeImage_SetOutputMessage(FreeImageErrorHandler);
      return true;
    }();

    std::string errors;
    do
    {
      FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.c_str(), 0);
      if(fif == FIF_UNKNOWN) 
        fif = FreeImage_GetFIFFromFilename(filename.c_str());
      if(fif == FIF_UNKNOWN)
      {
        errors = "unknown image type: " + filename;
        break;
      }

      if(FreeImage_FIFSupportsReading(fif))
        mBitmap = FreeImage_Load(fif, filename.c_str());
      if( mBitmap )
      {
        FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(mBitmap);
        if( bitmap32 )
        {
          FreeImage_Unload( mBitmap );
          mBitmap = bitmap32;
        }
        else
        {
          errors = "Image failed to convert to 32bits "
            "(non standard bitmap type ): " + filename;
          break;
        }
      }
      else
      {
        errors = "Image failed to load: " + filename;
        break;
      }

      mWidth = FreeImage_GetWidth(mBitmap);
      if(mWidth == 0)
      {
        errors = "Failed to image width: " + filename;
        break;
      }
      mHeight = FreeImage_GetHeight(mBitmap);
      if(mHeight == 0)
      {
        errors = "Failed to image height: " + filename;
        break;
      }

      mPitch = FreeImage_GetPitch(mBitmap);

      mBitsPerPixel= FreeImage_GetBPP(mBitmap);

      mBytes = FreeImage_GetBits(mBitmap);
      if(!mBytes)
      {
        errors = "Failed to get image data: " + filename;
        break;
      }

    } while (false);

    return errors;
  }
  Image::Image() : mBitmap(nullptr)
  {
    Clear();
  }
  Image::Image( const Image & rhs ):
    mBitmap(FreeImage_Clone(rhs.mBitmap)),
    mWidth(rhs.mWidth),
    mHeight(rhs.mHeight),
    mPitch(rhs.mPitch),
    mBitsPerPixel(rhs.mBitsPerPixel),
    mBytes(FreeImage_GetBits(mBitmap))
  {
  }
  Image::~Image()
  {
    Clear();
  }
  void Image::Clear()
  {
    if(mBitmap)
      FreeImage_Unload(mBitmap);
    mBitmap = nullptr;
    mWidth = 0;
    mHeight = 0;
    mPitch = 0;
    mBitsPerPixel = 0;
    mBytes = nullptr;
  }
  Image & Image::operator=( const Image & rhs )
  {
    Clear();
    mBitmap = FreeImage_Clone(rhs.mBitmap);
    mWidth = rhs.mWidth;
    mHeight = rhs.mHeight;
    mPitch = rhs.mPitch;
    mBitsPerPixel = rhs.mBitsPerPixel;
    mBytes = FreeImage_GetBits(mBitmap);
    return *this;
  }

  std::string Image::Save( const std::string & filepath )
  {
    std::string errors;

    FREE_IMAGE_FORMAT fif 
      = FreeImage_GetFIFFromFilename(filepath.c_str());
    if(fif == FIF_UNKNOWN)
    {
      errors = "Failed to save image ";
      errors += filepath;
      errors += "\n";
      errors += "Unknown image format";
    }
    else
    {
      bool success = 0 != FreeImage_Save(fif, mBitmap, filepath.c_str());

      if(!success)
      {
        errors = "Save failed (" + filepath + ")";
      }
    }
    return errors;
  }


} // Tac
