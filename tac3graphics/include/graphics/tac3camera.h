#pragma once
#include "tac3math\tac3vector3.h"
#include "tac3math\tac3vector4.h"
#include "tac3math\tac3matrix4.h"
#include "graphics\tacShader.h"

namespace Tac
{
  class Frustrum
  {
  public:
    enum Planes
    {
      eTop, eBottom, eLeft, eRight, eNear, eFar, eCount
    };
  private:
    // ax + by + cz + d = 0
    // normal faces outwards
    Vector4 mPlanes[Planes::eCount]; 
  };

  class Camera
  {
  public:
    Camera();

    void ComputeViewMatrix();
    Matrix4 mViewMatrix;
    Shader::Data mViewMatrixData;

    void ComputeInverseViewMatrix();
    Matrix4 mInvViewMatrix;
    Shader::Data mInvViewMatrixData;

    bool IsOrthographic() const;;

    void ComputePerspectiveProjMatrix();
    Matrix4 mPerspectiveProjMatrix;
    Shader::Data mPerspectiveProjMatrixData;

    void ComputeInversePerspectiveProjMatrix();
    Matrix4 mInvPerspectiveProjMatrix;
    Shader::Data mInvPerspectiveProjMatrixData;

    void ComputeOrthographicProjMatrix();
    Matrix4 mOrthoProjMatrix;
    Shader::Data mOrthoProjMatrixData;

    void GetCamDirections(Vector3 & right, Vector3 & up) const;

    bool mIsOrtho;

    float mNear;
    Shader::Data mNearData;

    float mFar;
    Shader::Data mFarData;

    float mFieldOfViewYRad;
    Shader::Data mFOVYRadData;

    float mAspectRatio;
    Shader::Data mAspectData;

    float mWidth;  // for orthographic proj
    Shader::Data mWidthData;

    float mHeight; // for orthographic proj
    Shader::Data mHeightData;

    Vector3 mPosition;
    Shader::Data mPosData;

    Vector3 mWorldSpaceUp;
    Shader::Data mWorldUpData;

    Vector3 mViewDirection;
    Shader::Data mViewDirData;
  };
}