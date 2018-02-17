#include "graphics\tac3light.h"
namespace Tac
{
  float Light::gInnerSpotlightAngle = 15.0f * 3.14f / 180.0f;
  Shader::Data Light::gInnerSpotlightAngleData( 
    GL_FLOAT, 1, "global inner spotlight angle", &gInnerSpotlightAngle);

  float Light::gOuterSpotlightAngle = 30.0f * 3.14f / 180.0f;
  Shader::Data Light::gOuterSpotlightAngleData(
    GL_FLOAT, 1, "global outer spotlight angle", &gOuterSpotlightAngle);

  float Light::gSpotFalloff = 1;
  Shader::Data Light::gSpotFalloffData( 
    GL_FLOAT, 1, "global spot falloff", &gSpotFalloff);

  Vector3 Light::gGlobalAmbient = Vector3(.2f, .2f, .2f);
  Shader::Data Light::gGlobalAmbientData(
    GL_FLOAT_VEC3, 1, "global light ambient", &gGlobalAmbient);

  Vector3 Light::gAttenuation = Vector3(1, 0.1f, 0);
  Shader::Data Light::gAttenuationData(
    GL_FLOAT_VEC3, 1, "global light attenuation", &gAttenuation);

  Light::Light() :
    mType(Type::eSpot),
    mTypeData(GL_INT, 1, "light type", &mType),
    mPos(0,0,5),
    mPosData(GL_FLOAT_VEC3, 1, "worldspace light pos", &mPos),
    mDir(0,0,-1),
    mDirData(GL_FLOAT_VEC3, 1, "worldspace light dir", &mDir),
    mDiffuse(.8f, .8f, .8f),
    mDiffuseData(GL_FLOAT_VEC3, 1, "light diffuse", &mDiffuse),
    mAmbient(0,0,0),
    mAmbientData(GL_FLOAT_VEC3, 1, "light ambient", &mAmbient),
    mSpecular(1,1,1),
    mSpecularData(GL_FLOAT_VEC3, 1, "light specular", &mSpecular),
    scale( 10.0f ),
    mScaleData( GL_FLOAT, 1, "light scale", &scale )
  {

  }

}