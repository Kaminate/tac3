#include "graphics\TacScene.h"
#include "graphics\tac3model.h"
#include "graphics\tac3light.h"
#include "graphics\tac3camera.h"

namespace Tac
{
  const std::string Scene::mDefaultWorld = "world";
  const std::string Scene::mDefaultUI = "ui";

  Scene::Scene() : mName("Unnamed scene")
  {

  }
  Scene::~Scene()
  {
    ClearModels();
    ClearCameras();
    ClearLights();
  }


  const std::string & Scene::GetName() const
  {
    return mName;
  }
  void Scene::SetName(const std::string & name)
  {
    mName = name;
  }


  void Scene::AddLight( Light* toAdd )
  {
    mLights.push_back(toAdd );
  }
  bool Scene::RemoveLight( Light * toRemove )
  {
    for(unsigned i = 0 ; i < mLights.size(); ++i)
    {
      if(mLights[ i ] == toRemove )
      {
        mLights[ i ] = mLights.back();
        mLights.pop_back();
        return true;
      }
    }
    return false;
  }
  void Scene::ClearLights()
  {
    mLights.clear();
  }
  std::vector<Light*> & Scene::GetLights()
  {
    return mLights;
  }


  Camera * Scene::CreateCamera()
  {
    Camera * toReturn = new Camera();
    mCameras.push_back(toReturn);
    return toReturn;
  }
  void Scene::DestroyCamera( Camera * toDestroy )
  {
    bool deleted = false;
    for(unsigned i = 0 ; i < mCameras.size(); ++i)
    {
      Camera * cur = mCameras[i];
      if(cur == toDestroy)
      {
        delete cur;
        mCameras[i] = mCameras.back();
        mCameras.pop_back();
        deleted = true;
        break;
      }
    }
    assert(deleted == true);
  }
  void Scene::ClearCameras()
  {
    for(auto * toDelete : mCameras)
      delete toDelete;
    mCameras.clear();
  }
  std::vector<Camera*> & Scene::GetCameras()
  {
    return mCameras;
  }


  std::vector<Model*> & Scene::GetModels()
  {
    return mModels;
  }
  void Scene::AddModel( Model* toAdd )
  {
    mModels.push_back( toAdd );
  }
  bool Scene::RemoveModel( Model * toRemove )
  {
    for(unsigned i = 0 ; i < mModels.size(); ++i)
    {
      if( mModels[ i ] == toRemove )
      {
        mModels[ i ] = mModels.back();
        mModels.pop_back();
        return true;
      }
    }
    return false;
  }
  void Scene::ClearModels()
  {
    mModels.clear();
  }


}
