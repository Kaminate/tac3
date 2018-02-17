
// structure based off KoRE rendering engine

#ifndef TAC_FRAMEBUFFER_STAGE_H_
#define TAC_FRAMEBUFFER_STAGE_H_

#include <string>
#include <vector>
//#include "graphics\TacRenderOperation.h"
//#include "graphics\TacShaderProgramPass.h"
//#include "graphics\TacGLIncludes.h"
//#include "graphics\TacResource.h"
namespace Tac
{
  class Framebuffer;
  class FramebufferStage // : public Resource
  {
  public:
    FramebufferStage();
    ~FramebufferStage();

    //void FramebufferStage::SetFramebuffer(
    //  const Framebuffer * buffer,
    //  const std::vector<GLenum> & activeAttachments);

    //virtual std::string ReloadAux() override;; // todo: remove

    //void AddPreRenderOperation(RenderOperation*);
    //void AddPostRenderOperation(RenderOperation*);
    //void AddShaderPass(ShaderPass*);

    bool & GetActiveRef();
    void SetName(const std::string & name);
    const std::string &  GetName() const;
  private:
    bool mActive;
    std::string mName;
    // owned by resource manager
    const Framebuffer * mFrameBuffer;
    //std::vector<GLenum> mActiveAttachments;

    // ownership
    friend class Renderer;
    //std::vector<RenderOperation* > mInternalPreRenderOps;
    //std::vector<RenderOperation* > mPreRenderOps;
    //std::vector<ShaderPass* > mShaderPasses;
    //std::vector<RenderOperation* > mPostRenderOps;
    //std::vector<RenderOperation* > mInternalPostRenderOps;
  };
}
#endif