#include "graphics\tac3camera.h"

namespace Tac
{
  Camera::Camera()
    : mNear(0.1f)
    , mFar(1000.0f)
    , mFieldOfViewYRad(3.14f / 2.0f)
    , mViewDirection(0,1,0)
    , mAspectRatio(4/3.0f)
    , mWorldSpaceUp(0,0,1)
    , mPosition(0,0,0)
    , mNearData(GL_FLOAT, 1, "near", &mNear)
    , mFarData(GL_FLOAT, 1, "far", &mFar)
    , mFOVYRadData(GL_FLOAT, 1, "fovyradians", &mFieldOfViewYRad)
    , mAspectData(GL_FLOAT, 1, "aspect", &mAspectRatio)
    , mWorldUpData(GL_FLOAT_VEC3, 1, "world up", &mWorldSpaceUp)
    , mPosData(GL_FLOAT_VEC3, 1, "world pos", &mPosition)
    , mViewDirData(GL_FLOAT_VEC3, 1, "viewDir", &mViewDirection)
    , mIsOrtho( false )
    , mViewMatrixData(GL_FLOAT_MAT4, 1, 
    "view Matrix", &mViewMatrix)
    , mInvViewMatrixData(GL_FLOAT_MAT4, 1, 
    "inv View Matrix", &mInvViewMatrix)
    , mPerspectiveProjMatrixData(GL_FLOAT_MAT4, 1, 
    "perspective Proj Matrix", &mPerspectiveProjMatrix)
    , mInvPerspectiveProjMatrixData(GL_FLOAT_MAT4, 1, 
    "inv Perspective proj matrix", &mInvPerspectiveProjMatrix)
    , mOrthoProjMatrixData(GL_FLOAT_MAT4, 1, 
    "ortho proj matrix", &mOrthoProjMatrix)
    , mWidth( 256 )
    , mHeight( 256 )
    , mWidthData( GL_FLOAT, 1, "width", &mWidth )
    , mHeightData( GL_FLOAT, 1, "height", &mHeight )
  {
    mViewMatrix.Identity();
    mInvViewMatrix.Identity();
    mPerspectiveProjMatrix.Identity();
    mInvPerspectiveProjMatrix.Identity();
    mOrthoProjMatrix.Identity();
  }

  void Camera::ComputeViewMatrix()
  {
    Vector3 camR(0,0,0);
    Vector3 camU(0,0,0);
    Vector3 negZ = -mViewDirection;
    GetCamDirections(camR, camU);

    Matrix4 worldToCamRot(
      camR.x, camR.y, camR.z, 0,
      camU.x, camU.y, camU.z, 0,
      negZ.x, negZ.y, negZ.z, 0,
      0,0,0,1);

    Matrix4 worldToCamTra(
      1,0,0, -mPosition.x,
      0,1,0, -mPosition.y,
      0,0,1, -mPosition.z,
      0,0,0,1);

    mViewMatrix = worldToCamRot * worldToCamTra;
  }

  void Camera::ComputePerspectiveProjMatrix()
  {
    //                                       /----- [x,y,z]
    //                          [x'y'z']/----
    //                          /----|
    //                    /----/     |
    //               /---/           | projHeight / 2
    //           /--/   theta        |
    //    camera/--------------------|
    //                     d

    // map the x from (-1, 1) by divinding by projWidth

    float theta = mFieldOfViewYRad / 2.0f;

    // note:           
    // we ignored sX, sY, A, B for simplicity
    // P' (PROJ)  PROJ                         P( CAM )
    // [-dx/z] = [-dx/z] / w = [xd] = [d 0 0 0][x]
    // [-dy/z] = [-dy/z] / w = [yd] = [0 d 0 0][y]
    // [-d]    = [-dz/z] / w = [zd] = [0 0 d 0][Z]
    // [1]     = [1]     / w = [-z] = [0 0-1 0][1]

    // I believe that projection is 4D, 
    // Once you divide by w, you get NDC (3D).    

    float cotTheta = 1.0f / tan(theta);
    // sX = d / (pW / 2) = cot(theta) / aspectRatio
    float sX  =  cotTheta / mAspectRatio; // maps x to -1, 1

    // sY = d / (pH / 2)
    float sY = cotTheta; // maps y to -1, 1

    float A = -(mFar +mNear) / (mFar - mNear);    // maps -n,-f to -1,1
    float B = -2 * mFar * mNear / (mFar - mNear); // maps -n,-f to -1,1

    mPerspectiveProjMatrix = Matrix4(
      sX,0,0,0,  
      0,sY,0,0,  
      0,0,A,B,   
      0,0,-1,0); 
  }

  void Camera::ComputeInverseViewMatrix()
  {
    Vector3 camR(0,0,0);
    Vector3 camU(0,0,0);
    Vector3 negZ = -mViewDirection;
    GetCamDirections(camR, camU);

    Matrix4 camToWorRot(
      camR.x, camU.x, negZ.x, 0,
      camR.y, camU.y, negZ.y, 0,
      camR.z, camU.z, negZ.z, 0,
      0,0,0,1);

    Matrix4 camToWorTra(
      1,0,0, mPosition.x,
      0,1,0, mPosition.y,
      0,0,1, mPosition.z,
      0,0,0,1);

    mInvViewMatrix = camToWorTra * camToWorRot;
  }

  void Camera::GetCamDirections( Vector3 & right, Vector3 & up ) const
  {
    if(mViewDirection == Vector3(0,0,1))
    {
      right = Vector3(1,0,0);
      up = Vector3(0,1,0);
      return;
    }

    if (mViewDirection.IsAlignedWith(mWorldSpaceUp))
    {
      Vector3 arbitrayVector(0,-1,0);
      if (mViewDirection.IsAlignedWith(arbitrayVector))
        arbitrayVector = Vector3(0,1,0);

      right = mViewDirection.Cross(arbitrayVector);
    }
    else
      right = mViewDirection.Cross(mWorldSpaceUp);

    right.Normalize(); // oops. this is important
    up = right.Cross(mViewDirection);
  }

  void Camera::ComputeOrthographicProjMatrix()
  {
    float a = -2/(mFar - mNear);
    float b = -(mNear + mFar) / (mFar - mNear);

    /*
     ___________
    |           |^
    |           ||
    |     +     |h
    |           ||
    |___________|v
    <-----w----->

    x: aligns -w/2 to -1, w/2 to 1
    y: aligns -h/2 to -1, h/2 to 1
    z: aligns -near to -1, -far to 1

    */

    mOrthoProjMatrix =  Matrix4(
      2/mWidth, 0, 0, 0,
      0, 2/mHeight, 0, 0,
      0, 0, a, b,
      0, 0, 0, 1);
  }

  // http://allenchou.net/2014/02/game-math-how-to-eyeball-the-inverse-of-a-matrix/
  void Camera::ComputeInversePerspectiveProjMatrix() 
  {
    float theta = mFieldOfViewYRad / 2.0f;
    float cotTheta = 1.0f / tan(theta);
    float sX  =  cotTheta / mAspectRatio; // maps x to -1, 1
    float sY = cotTheta; // maps y to -1, 1
    float A = (-mNear -mFar) / (mFar - mNear);    // maps -n,-f to -1,1
    float B = -2 * mNear * mFar / (mFar - mNear); // maps -n,-f to -1,1

    mInvPerspectiveProjMatrix = Matrix4(
      1/sX,0,0,0,
      0,1/sY,0,0,
      0,0,0,-1,
      0,0,1/B,A/B);
  }

  bool Camera::IsOrthographic() const
  {
    return mIsOrtho;
  }

}

