#pragma once
#include "tac3math\tac3vector3.h"
#include "tac3math\tac3matrix4.h"
#include "graphics\tacShader.h"
namespace Tac
{
  class Geometry;
  class Texture;
  class Model
  {
  public:
    Model();
    void ComputeWorld();
    void ComputeInverseTransposeWorld();
    Vector3 mPos;
    Vector3 mScale;
    Vector3 mRot;
    Matrix4 mWorldMatrix;
    Matrix4 mInvTransWorldMatrix;
    Shader::Data mWorldMatrixData;
    Shader::Data mInvTransWorldMatrixData;

    void SetGeometry(Geometry * geom);
    Geometry * GetGeometry();

    Vector3 mTEMPCOLOR;
    Shader::Data mTEMPCOLORData;
    Texture * mTEMP_DIFFUSE_TEX; // owned by resource manager
    Texture * mTEMP_SPECULAR_TEX; // owned by resource manager
    Texture * mTEMP_NORMAL_TEX; // owned by resource manager
    //bool mTEMP_VISIBLE;

  private:
    Geometry * mGeometry; // owned by resource manager
  };
}