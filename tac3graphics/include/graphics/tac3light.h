#pragma  once
#include "tac3math\tac3vector3.h"
#include "graphics\tacShader.h"
namespace Tac
{
  class Light
  {
  public:
    Light();
    enum class Type : uint32_t {
      ePoint,
      eSpot,
      eDir,
      eCount} mType;
    Shader::Data mTypeData;

    float scale;
    Shader::Data mScaleData;

    Vector3 mPos; // spot, point
    Shader::Data mPosData;

    Vector3 mDir; // spot, directional
    Shader::Data mDirData;

    Vector3 mDiffuse; // all
    Shader::Data mDiffuseData;

    Vector3 mAmbient; // all
    Shader::Data mAmbientData;

    Vector3 mSpecular; // all
    Shader::Data mSpecularData;

    static float gInnerSpotlightAngle; // spot
    static Shader::Data gInnerSpotlightAngleData;

    static float gOuterSpotlightAngle; // spot
    static Shader::Data gOuterSpotlightAngleData;

    static float gSpotFalloff; // spot
    static Shader::Data gSpotFalloffData;

    static Vector3 gGlobalAmbient; // all
    static Shader::Data gGlobalAmbientData;

    static Vector3 gAttenuation; // point, spot
    static Shader::Data gAttenuationData;
  };
}