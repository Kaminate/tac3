#include "graphics\TacRenderer.h"
#include "graphics\tac3model.h"
#include "graphics\TacTechnique.h"
#include "graphics\TacScene.h"
#include "graphics\tac3geometry.h"
#include <algorithm>

namespace Tac
{

  Renderer::Renderer() :
    windowWidth(0),
    windowHeight(0),
    mClearColor(.1f, .1f, .1f, 1.0f)
  {

  }
  Renderer::~Renderer()
  {
    ClearTechniques();
    ClearScenes();
  }
  void Renderer::ClearTechniques()
  {
    for(auto & pair : mTechniques)
    {
      delete pair.second;
    }

    mTechniques.clear();
  }
  void Renderer::ClearScenes()
  {
    for(auto & pair : mScenes)
    {
      delete pair.second;
    }
    mScenes.clear();
  }
  Scene * Renderer::GetScene( const std::string & name )
  {
    Scene * toReturn = nullptr;
    auto it = mScenes.find(name);
    if(it != mScenes.end())
      toReturn = (*it).second;
    return toReturn;
  }
  Technique * Renderer::GetTechnique( const std::string & name )
  {
    Technique * toReturn = nullptr;
    auto it = mTechniques.find(name);
    if(it != mTechniques.end())
      toReturn = (*it).second;
    return toReturn;
  }
  void Renderer::AddTechnique( Technique * newTechnique )
  {
#ifdef _DEBUG
    auto it = mTechniques.find(newTechnique->GetName());
    assert(it == mTechniques.end());
#endif
    mTechniques[newTechnique->GetName()] = newTechnique;
    newTechnique->SetRenderer(this);
  }
  void Renderer::RemTechnique( Technique * toRemove )
  {
#if _DEBUG
    if( !toRemove )
      __debugbreak();
#else
    assert( toRemove );
#endif

    mTechniques.erase( toRemove->GetName() );
  }
  //void Renderer::RenderFrame()
  //{
  //  glClearColor(
  //    mClearColor.x,
  //    mClearColor.y,
  //    mClearColor.z,
  //    mClearColor.w);
  //  glClearDepthf(1.0f);
  //  glEnable(GL_DEPTH_TEST);
  //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //  glViewport(0,0,windowWidth,windowHeight);

  //  std::vector<Technique*> allTechniques;
  //  allTechniques.reserve(mTechniques.size());
  //  for(auto & pair : mTechniques)
  //    allTechniques.push_back(pair.second);

  //  std::sort(allTechniques.begin(), allTechniques.end(),
  //    [](Technique * lhs, Technique * rhs){
  //      return lhs->GetPriority() < rhs->GetPriority();
  //  });

  //  for(Technique * curTechnique : allTechniques)
  //    curTechnique->Execute();
  //}
  void Renderer::RenderFrame( std::map< std::string, Scene* > & scenes )
  {
    glClearColor(
      mClearColor.x,
      mClearColor.y,
      mClearColor.z,
      mClearColor.w );
    glClearDepthf( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glViewport( 0, 0, windowWidth, windowHeight );

    std::vector< Technique* > allTechniques;
    allTechniques.reserve(mTechniques.size());
    for(auto & pair : mTechniques)
      allTechniques.push_back(pair.second);

    std::sort(allTechniques.begin(), allTechniques.end(),
      [](Technique * lhs, Technique * rhs){
        return lhs->GetPriority() < rhs->GetPriority();
    });

    for(Technique * curTechnique : allTechniques)
      curTechnique->Execute( scenes );
  }

  void Renderer::AddScene( Scene * newScene )
  {
#ifdef _DEBUG
    auto it = mScenes.find(newScene->GetName());
    assert(it == mScenes.end()); // already added
#endif

    mScenes[newScene->GetName()] = newScene;
  }
  void Renderer::RemoveScene( Scene * toRemove )
  {
    mScenes.erase(toRemove->GetName());
  }
  void Renderer::Resize( int width, int height )
  {
    windowWidth = width;
    windowHeight = height;
  }
  int Renderer::GetWindowWidth()const
  {
    return windowWidth;
  }
  int Renderer::GetWindowHeight()const
  {
    return windowHeight;
  }
  ResourceManager & Renderer::GetResourceManager()
  {
    return mResourceManager;
  }
  Vector4 & Renderer::GetClearColor()
  {
    return mClearColor;
  }
}