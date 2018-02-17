#include "graphics\tac3acceleration.h"
#include "graphics\tac3model.h"
namespace Tac
{
  Model::Model() : 
    mPos(0,0,0),
    mRot(0,0,0), 
    mScale(1,1,1),
    mWorldMatrixData(GL_FLOAT_MAT4, 1, 
    "world matrix", &mWorldMatrix),
    mInvTransWorldMatrixData(GL_FLOAT_MAT4, 1, 
    "inverse transpose world matrix", &mInvTransWorldMatrix),
    mGeometry(nullptr),
    mTEMPCOLOR(0,1,0),
    mTEMPCOLORData(GL_FLOAT_VEC3, 1, "TODO: move this to material", &mTEMPCOLOR),
    mTEMP_DIFFUSE_TEX(nullptr),
    mTEMP_NORMAL_TEX(nullptr),
    mTEMP_SPECULAR_TEX(nullptr)
    //mTEMP_VISIBLE(true)
  {
    mWorldMatrix.Identity();
    mInvTransWorldMatrix.Identity();
  }

  void Model::ComputeWorld()
  {
    mWorldMatrix = Matrix4::Transform(mScale, mRot, mPos);
  }

  void Model::ComputeInverseTransposeWorld()
  {
    mInvTransWorldMatrix = Matrix4::TransformInv(mScale, mRot, mPos);
    mInvTransWorldMatrix.Transpose();
  }

  void Model::SetGeometry(Geometry * geom)
  {
    mGeometry = geom;
  }

  Geometry * Model::GetGeometry()
  {
    return mGeometry;
  }
}
