#include "graphics\TacFramebufferStage.h"
//#include "graphics\TacBindFramebuffer.h"

namespace Tac
{
  FramebufferStage::FramebufferStage() 
    : mName("Unnamed Framebuffer Stage")
    , mActive(true)
    , mFrameBuffer(nullptr)
  {

  }



  FramebufferStage::~FramebufferStage()
  {
#define clearthing(TreeCreateThing)\
  for(auto p : TreeCreateThing) {delete p;} TreeCreateThing.clear();

    //clearthing(mPreRenderOps);
    //clearthing(mPostRenderOps);
    //clearthing(mShaderPasses);
    //clearthing(mInternalPreRenderOps);
    //clearthing(mInternalPostRenderOps);
  }
  /*

  void FramebufferStage::SetFramebuffer(
    const Framebuffer * buffer,
    const std::vector<GLenum> & activeAttachments)
  {
    mFrameBuffer = buffer;
    mActiveAttachments = activeAttachments;

    BindFramebuffer * bindFBOop 
      = new BindFramebuffer(buffer, activeAttachments);
    mInternalPreRenderOps.push_back(bindFBOop);
  }

  std::string FramebufferStage::ReloadAux()
  {
    return "";
  }

  void FramebufferStage::AddPreRenderOperation( RenderOperation* op)
  {
    mPreRenderOps.push_back(op);
  }

  void FramebufferStage::AddPostRenderOperation( RenderOperation* op)
  {
    mPostRenderOps.push_back(op);
  }

  void FramebufferStage::AddShaderPass( ShaderPass* pass)
  {
    mShaderPasses.push_back(pass);
  }
  */

  bool & FramebufferStage::GetActiveRef()
  {
    return mActive;
  }

  void FramebufferStage::SetName( const std::string & name )
  {
    mName = name;
  }

  const std::string & FramebufferStage::GetName() const
  {
    return mName;
  }



}  // Tac
