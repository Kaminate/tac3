#ifndef TAC_RENDERER_H
#define TAC_RENDERER_H
#include <vector>
#include <map>
#include <set>
#include "graphics\TacResourceManger.h"
#include "tac3math\tac3vector4.h"

namespace Tac
{
  class Scene;
  class Technique;
  struct ResourceManager;

  class Renderer
  {
  public:
    Renderer();
    ~Renderer();

    //void RenderFrame();
    void RenderFrame( std::map< std::string, Scene* > & scenes);
    void ClearTechniques();
    void ClearScenes();

    Scene * GetScene(const std::string & name);
    void AddScene(Scene * newScene);
    void RemoveScene(Scene * toRemove); // does not delete


    Technique * GetTechnique(const std::string & name);
    void AddTechnique(Technique * newTechnique);

    // does not delete
    void RemTechnique(Technique * toRemove);

    void Resize(int width, int height);
    int GetWindowWidth()const;
    int GetWindowHeight()const;
    ResourceManager & GetResourceManager();
    Vector4 & GetClearColor();


  private:
    std::map<std::string, Scene*> mScenes; // owned
    std::map<std::string, Technique*> mTechniques; // owned
    int windowWidth;
    int windowHeight;
    Vector4 mClearColor;
    ResourceManager mResourceManager;
  };
}

#endif  
