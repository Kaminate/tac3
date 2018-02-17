#pragma  once
#include <vector>
#include <string>
namespace Tac
{
  class Light;
  class Model;
  class Camera;
  class Scene
  {
  public:
    static const std::string mDefaultWorld;
    static const std::string mDefaultUI;
    Scene();
    ~Scene();
    const std::string & GetName() const;
    void SetName(const std::string & name);

    void AddLight( Light* toAdd );
    bool RemoveLight( Light * toRemove );
    void ClearLights();
    std::vector<Light*> & GetLights();

    Camera * CreateCamera();
    void DestroyCamera( Camera * toDestroy );
    void ClearCameras();
    std::vector<Camera*> & GetCameras();

    void AddModel( Model* toAdd );
    bool RemoveModel( Model* toRemove );
    void ClearModels();
    std::vector<Model*> & GetModels();

  private:
    std::vector<Camera*> mCameras; // owned
    std::vector<Light*> mLights; // not owned
    std::vector<Model*> mModels; // not owned

    std::string mName;
  };

}
