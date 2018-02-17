//#pragma once
//#include "graphics\tac3acceleration.h"
//#include "graphics\TacRenderer.h"
//#include "core\tac3systemApplication.h"
//#include "graphics\tac3light.h"
//#include "core\tacWorldRenderTechnique.h"
//
//namespace Tac
//{
//  class Light;
//  class Model;
//  class TweakBar;
//  class Camera;
//  class CS300_2 : public SystemApplication
//  {
//  public:
//    CS300_2();
//    virtual ~CS300_2();
//    virtual std::string Init() override;
//    virtual void Update(float dt) override;
//    virtual void UnInit() override;
//  private:
//    NormalRender * mNormalRender;
//
//    int mMaxLights;
//
//    void OnAddLight();
//    void AddLightt(Light::Type lightType);
//
//    void RemoveLight();
//    void Rotate();
//    void SetTextures();
//    void RemoveTextures();
//
//    enum Shape{ eBunny, eHorse, eSphere, eCube, eCount
//    } currShape, lastShape;
//    struct Data
//    {
//      Vector3 rot;
//      Vector3 scale;
//      Vector3 pos;
//      Geometry * geom;
//    } mData[ Shape::eCount ];
//    void SetShape( Shape );
//
//    class WorldRender * mWorldRender;
//    std::string LoadCubemap(
//      const char * folder,
//      Texture * faces[6]);
//    Light::Type mLightTypeToAdd;
//    std::vector<Light*> mLights;
//    std::vector<Model*> mLightModels;
//    TweakBar * mTweakbar;
//
//    Camera * mCamera;
//    std::vector<Model*> mFloatingModels;
//    Model * mModel;
//    Model * mGround;
//
//    enum class CubemapType { eNone, eTest, eLangholmen, eCount
//    } mCurrCubemap, mLastCubemap;
//    struct CubemapTextures
//    {
//      Texture * faces[ 6 ];
//      const char * folder;
//    } cubemaps[ CubemapType::eCount ];
//    void SetCubemap( CubemapType );
//
//    Vector3 mInitialCamPos;
//    Scene * world;
//    Scene * ui;
//    Texture * mDiffuseTexture;
//    Texture * mSpecularTexture;
//    Texture * mNormalTexture;
//    float mCameraRadius;
//    float mCameraAngle;
//    float mCameraRadPerSec;
//    float mLightRadius;
//    float mLightAngle;
//    float mLightRadPerSec;
//    float mLightAmplitude;
//    int mLightBumps;
//  };
//} // Tac
