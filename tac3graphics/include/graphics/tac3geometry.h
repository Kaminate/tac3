#pragma once
#include "tac3math\tac3vector4.h"
#include "tac3math\tac3vector3.h"
#include "tac3math\tac3vector2.h"
#include "tac3math\tac3boundingvolume.h"
#include "graphics\tac3gl.h"
#include <map>
#include <vector>

namespace Tac
{
  class BoundingVolumes
  {
  public:
    BoundingVolumes() : initialized( false ){}
    std::string Save( const std::string& filepath );
    std::string Load( const std::string& filepath );
    void Generate( const std::vector< Vector3 >& positions );
    BoundingSphere mSphereCentroid;
    BoundingSphere mSphereRitter;
    BoundingSphere mSphereLarsson;
    BoundingSphere mSpherePCA;
    BoundingEllipsoid mEllipsoidPCA;
    OBB mOBBPCA;
    AABB mAABB;

    bool initialized;
  };
  class Geometry
  {    
  public:

    // correspond to binding points in the vao
    enum Attribute
    {
      ePos = 0,
      eCol = 1, 
      eUvs = 2, 
      eNor = 3 , 
      eBoneId = 4 , 
      eBoneWeight = 5,
      eTangents = 6,
      eBitangents = 7,
      eCount
    };

    Geometry();
    ~Geometry();
    void Clear();
    std::string Load(const std::string & fileName);
    void LoadAllAttribs();
    bool Save(const std::string & fileName);
    void LoadCube( float halfWidth );
    void LoadSphere(float radius, int numDivisions);
    void LoadTriangle(float length);
    void LoadNDCQuad();

    struct PerComponentData
    {
      int mComponentsPerVert; // for Attribute::ePos - 3 floats
      int mComponentSize;     // for Attribute::ePos - 4 = sizeof( float )
      GLenum mComponentType;  // for Attribute::ePos - GL_FLOAT
    };
    static const PerComponentData Data[eCount];

    std::vector< Vector3 > mPos;
    std::vector< Vector3 > mCol;
    std::vector< Vector2 > mUVs;
    std::vector< Vector3 > mNor;
    std::vector< Vector4uint > mBoneIds;
    std::vector< Vector4 > mBoneWeights;
    std::vector< Vector3 > mTangents;
    std::vector< Vector3 > mBitangents;
    std::vector< unsigned > mIndexes;

    unsigned GetVertexCount() const;
    unsigned GetIndexCount() const;

    const std::string & GetName() const;
    void SetName(const std::string & name);

    GLuint GetVAO();
    GLuint GetIndexBuffer();

    GLenum mPrimitive;

    BoundingVolumes mBoundingVolumes;

  private:
    std::string mName; // not filepath
    GLuint mFBOs[eCount];
    GLuint mIndexBuffer;
    GLuint mVAO;
  };

}

