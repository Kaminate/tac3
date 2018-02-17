#pragma once
#include "tac3system.h"
#include "tac3component.h"
#include "tac3util\tac3utilities.h"
#include <set>

namespace Tac
{
  class Renderer;
  class SystemGraphics;
  class Camera;
  class Model;
  class Light;
  class Geometry;

  class ModelComponent : public Component
  {
  public:
    ModelComponent();
    ~ModelComponent();

    void AddToTweakBar( TweakBar * bar ) override;
    void RemoveFromTweakBar( TweakBar * bar ) override;
    void OnAddGeometry();
    void OnAddDiffuse();
    void OnAddSpecular();
    void OnComputeBoundingVolumes();
    void Save( Saver & filesaver ) const override;

    // todo(n8): make these return error if scene not found
    void Load( Loader & loader ) override;

    void OnMove() override;
    Model* GetModel();
    void SetSceneName( const std::string& sceneName );


  private:
    std::string LoadGeometry();
    std::string LoadDiffuse();
    std::string LoadSpecular();
    std::string mRenderSceneName;
    std::string mGeometryName;
    std::string mDiffuseName;
    std::string mSpecularName;
    Model * mModel;
  };

  class CameraComponent : public Component
  {
  public:
    CameraComponent();
    ~CameraComponent();

    void AddToTweakBar(TweakBar * bar) override;
    void RemoveFromTweakBar(TweakBar * bar) override;
    void Save( Saver & filesaver ) const override;
    void Load( Loader & loader ) override;
    void OnMove() override;
    Camera* GetCamera();

  private:
    std::string mSceneName;
    Camera * mCamera;
  };

  class LightComponent : public Component
  {
  public:
    LightComponent();
    ~LightComponent();

    void AddToTweakBar( TweakBar* bar ) override;
    void RemoveFromTweakBar( TweakBar* bar ) override;
    void Save( Saver& fileSaver ) const override;
    void Load( Loader& fileLoader ) override;
    void OnMove() override;
    Light* GetLight();
    void SetSceneName(const std::string& sceneName );
  private:
    std::string mRenderSceneName;
    Light* mLight;
  };

  class SystemGraphics : public System
  {
  public:
    SystemGraphics();
    ~SystemGraphics();
    std::string Init() override;

    void Update( float dt ) override;
    Renderer * GetRenderer();

  private:
    Renderer * mRenderer;
    float msecsBetweenShaderReloads;
    float msecsTillNextShaderReload;
    //std::vector< ModelComponent > mModelComponoents;
  };
} // Tac
