#include "graphics\TacRendererState.h"
//#include "graphics\TacComponentModel.h"

namespace Tac
{

  RendererState::RendererState()
    //: mModel(nullptr)
    : mFrameBuffer(nullptr)
  {

  }

  //const ModelComponent * RendererState::GetModel()const
  //{
  //  return mModel;
  //}

  //void RendererState::SetModel( const ModelComponent * currmodel )
  //{
  //  mModel = currmodel;
  //}

  void RendererState::SetFramebuffer(const Framebuffer*buffer )
  {
    mFrameBuffer=buffer;
  }

  const Framebuffer* RendererState::GetFramebuffer()const
  {
    return mFrameBuffer;
  }


}