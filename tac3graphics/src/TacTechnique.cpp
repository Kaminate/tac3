#include "graphics\TacTechnique.h"
#include "graphics\tac3framebuffer.h"
#include "graphics\tac3texture.h"
#include "graphics\TacRenderer.h"
#include <graphics\TacScene.h>
namespace Tac
{
  Technique::Technique(const std::string & name) :
    mName(name),
    mPriority(0),
    myRenderer(nullptr),
    mRenderTo( nullptr ),
    mShader( nullptr ),
    mSceneName( "" )
  {

  }

  Technique::~Technique()
  {

  }
  const std::string & Technique::GetName() const
  {
    return mName;
  }
  void Technique::SetPriority( float priority )
  {
    mPriority = priority;
  }
  float Technique::GetPriority() const
  {
    return mPriority;
  }
  void Technique::SetRenderer( Renderer * renderer )
  {
    myRenderer = renderer;
  }
  Renderer * Technique::GetRenderer()
  {
    return myRenderer;
  }
  void Technique::Execute(
    std::map< std::string, Scene* > & scenes )
  {
    scenes;
  }

  void Technique::SetSceneName( const std::string& sceneName )
  {
    mSceneName = sceneName;
  }

  void Technique::SetFramebuffer( Framebuffer* renderTo )
  {
    mRenderTo = renderTo;
  }

  void Technique::SetShader( Shader* myShader )
  {
    mShader = myShader;
  }

  const std::string& Technique::GetSceneName()
  {
    return mSceneName;
  }

  Framebuffer* Technique::GetFramebuffer()
  {
    return mRenderTo;
  }

  Shader* Technique::GetShader()
  {
    return mShader;
  }

  void Technique::GetFramebufferDimensions( int* width, int* height )
  {
    if( mRenderTo->HasTexture( GL_COLOR_ATTACHMENT0 ) )
    {
      Texture * colorTex = mRenderTo->GetTexture( GL_COLOR_ATTACHMENT0 );
      const Texture::Properties & props = colorTex->GetProperties();
      *width = props.mWidth;
      *height = props.mHeight;
    }
    else
    {
      *width = myRenderer->GetWindowWidth();
      *height = myRenderer->GetWindowHeight();
    }
  }

  Camera* Technique::GetCamera( Scene* myScene )
  {
    Camera* result = nullptr;
    if( myScene )
    {
      auto & cameras = myScene->GetCameras();
      if( !cameras.empty() )
        result = cameras.front();
    }
    return result;
  }
  Scene* Technique::GetScene( std::map< std::string, Scene* >& scenes )
  {
    Scene* result = nullptr;

    auto it = scenes.find( GetSceneName() );
    if( it != scenes.end() )
      result = (*it).second;
    return result;
  }
} // namespace
