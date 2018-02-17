#include "graphics\tac3gl.h"
namespace Tac
{
  void CheckOpenGLErrors()
  {
    const char * errorMessage = 0;
    switch(glGetError())
    {
    case GL_NO_ERROR:
      // No error has been recorded. 
      // The value of this symbolic constant is guaranteed to be 0.
      break;

    case GL_INVALID_ENUM:
      errorMessage = 
        "GL_INVALID_ENUM                                               \n"
        "An unacceptable value is specified for an enumerated argument.\n"
        "The offending command is ignored and has no other side effect \n"
        "than to set the error flag.                                   \n";
      break;                                                                

    case GL_INVALID_VALUE:                                                  
      errorMessage =                                                        
        "GL_INVALID_VALUE                                              \n"
        "A numeric argument is out of range.                           \n"
        "The offending command is ignored and has no other side effect \n"
        "than to set the error flag.                                   \n";
      break;                                                                

    case GL_INVALID_OPERATION:                                              
      errorMessage =                                                        
        "GL_INVALID_OPERATION                                          \n"
        "The specified operation is not allowed in the current state.  \n"
        "The offending command is ignored and has no other side effect \n"
        "than to set the error flag.                                   \n"
        "\n"
        "Note:   If this happened during glBindTexture, make sure that \n"
        "you're using the right shader in gluniform1i                  \n";
      break;                                                                

    case GL_INVALID_FRAMEBUFFER_OPERATION:                                  
      errorMessage =                                                        
        "GL_INVALID_FRAMEBUFFER_OPERATION                                     \n"
        "The framebuffer object is not complete.                              \n"
        "The offending command is ignored and has no other side effect        \n"
        "than to set the error flag.                                          \n";
      break;

    case GL_OUT_OF_MEMORY:
      errorMessage = 
        "GL_OUT_OF_MEMORY                                                     \n"
        "There is not enough memory left to execute the command.              \n"
        "The state of the GL is undefined, except for the state of the error  \n"
        "flags, after this error is recorded.                                 \n";
      break;

    case GL_STACK_UNDERFLOW:
      errorMessage = 
        "GL_STACK_UNDERFLOW                                                   \n"
        "An attempt has been made to perform an operation that would cause an \n"
        "internal stack to underflow.                                         \n";
      break;

    case GL_STACK_OVERFLOW:
      errorMessage = 
        "GL_STACK_OVERFLOW                                                    \n"
        "An attempt has been made to perform an operation that would cause an \n"
        "internal stack to overflow.                                          \n";
      break;
    }
#ifdef _DEBUG
    if(errorMessage)
      __debugbreak();
      // "An OpenGL error has occured: \n %s", errorMessage);
#endif
  }

  void CheckFramebufferStatus()
  {
    const char * errorMessage = 0;
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
    {
    case GL_FRAMEBUFFER_COMPLETE :
      break;
    case GL_FRAMEBUFFER_UNDEFINED:
      errorMessage = "GL_FRAMEBUFFER_UNDEFINED \n"
        "target is the default framebuffer, \n"
        "but the default framebuffer does not exist.";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      errorMessage = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT \n"
        "any of the framebuffer attachment points are framebuffer incomplete.";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      errorMessage = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT \n"
        "the framebuffer does not have at least one image attached to it \n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      errorMessage = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER \n"
        "the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for  \n"
        "any color attachment point(s) named by GL_DRAW_BUFFERi \n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      errorMessage = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER \n"
        "GL_READ_BUFFER is not GL_NONE and the value of  \n"
        "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for  \n"
        "the color attachment point named by GL_READ_BUFFER \n";
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      errorMessage = "GL_FRAMEBUFFER_UNSUPPORTED \n"
        "the combination of internal formats of the attached images violates  \n"
        "an implementation-dependent set of restrictions. \n";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
      errorMessage = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE \n"
        "the value of GL_RENDERBUFFER_SAMPLES is not the same for all  \n"
        "attached renderbuffers;  \n"
        "if the value of GL_TEXTURE_SAMPLES is the not same for all  \n"
        "attached textures;  \n"
        "or, if the attached images are a mix of renderbuffers and textures, \n"
        "the value of GL_RENDERBUFFER_SAMPLES does not match \n"
        "the value of GL_TEXTURE_SAMPLES. \n"
        "\n"
        "\n"
        "OR \n"
        "\n"
        "\n"
        "the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all \n"
        "attached textures; \n"
        "or, if the attached images are a mix of renderbuffers and textures, \n"
        "the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE \n"
        "for all attached textures.";
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
      errorMessage = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS \n"
        "any framebuffer attachment is layered, \n"
        "and any populated attachment is not layered, \n"
        "or if all populated color attachments are not from textures \n"
        "of the same target.";
      break;
    default:
#ifdef _DEBUG
      __debugbreak();// ( "A case wasn't handeled");
#endif // _DEBUG
      break;
    }
#ifdef _DEBUG
    if(errorMessage)
      __debugbreak(); // "OpenGL framebuffer error: \n %s", errorMessage);
#endif
  }

  void CheckglMapBufferRangeErrors()
  {
    const char * errorMessage = 0;
    switch(glGetError())
    {
    case GL_NO_ERROR:
      // No error has been recorded. 
      // The value of this symbolic constant is guaranteed to be 0.
      break;

    case GL_INVALID_VALUE:                                                  
      errorMessage =                                                        
        "GL_INVALID_VALUE                                                     \n"
        "GL_INVALID_VALUE is generated if either of offset or length is       \n"
        "negative, or if offset + length is greater than the value of         \n"
        "GL_BUFFER_SIZE.                                                      \n"
        "                                                                     \n"
        "GL_INVALID_VALUE is generated if access has any bits set other than  \n"
        "those defined above.                                                 \n";
      break;                                                                

    case GL_INVALID_OPERATION:                                              
      errorMessage =                                                        
        "GL_INVALID_OPERATION                                                 \n"
        "The buffer is already in a mapped state.                             \n"
        "                                                                     \n"
        "Neither GL_MAP_READ_BIT or GL_MAP_WRITE_BIT is set.                  \n"
        "                                                                     \n"
        "GL_MAP_READ_BIT is set and any of GL_MAP_INVALIDATE_RANGE_BIT,       \n"
        "GL_MAP_INVALIDATE_BUFFER_BIT, or GL_MAP_UNSYNCHRONIZED_BIT is set.   \n"
        "                                                                     \n"
        "GL_MAP_FLUSH_EXPLICIT_BIT is set and GL_MAP_WRITE_BIT is not set.    \n";
      break;                                                                

    case GL_OUT_OF_MEMORY:
      errorMessage = 
        "GL_OUT_OF_MEMORY                                                     \n"
        "GL_OUT_OF_MEMORY is generated if glMapBufferRange fails because      \n"
        "memory for the mapping could not be obtained                         \n";
      break;

    }
#ifdef _DEBUG
    if(errorMessage)
      __debugbreak();
      // "An OpenGL error has occured"
      //" during the function glMapBufferRangeErrors: \n %s", errorMessage);
#endif
  }
} // Tac
