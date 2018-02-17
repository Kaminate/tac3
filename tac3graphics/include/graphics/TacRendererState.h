#ifndef TAC_RENDERER_STATE_H
#define TAC_RENDERER_STATE_H

namespace Tac
{
  //class ModelComponent;
  class Framebuffer;
  class RendererState
  {
  public:
    RendererState();

    //void SetModel(const ModelComponent * curModel);
    //const ModelComponent * GetModel()const;

    void SetFramebuffer(const Framebuffer*buffer);;
    const Framebuffer*GetFramebuffer()const;

  private:

    // set by Renderer::Render()
    //const ModelComponent * mModel;

    // set by BindFramebuffer::Execute()
    const Framebuffer * mFrameBuffer;
  };
}

#endif  
