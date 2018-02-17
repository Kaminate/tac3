#pragma once
//#define GLEW_STATIC
//#include "GL\glew.h"
#include "graphics\gl_core_4_4.h"
#include "gl\GL.h"
namespace Tac
{
  // returns invalid operation if gl context isnt created
  void CheckOpenGLErrors();
  void CheckFramebufferStatus();
  void CheckglMapBufferRangeErrors();
}
