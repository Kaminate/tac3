#pragma once

#include "tac3vector3.h"
#include <vector>

namespace Tac
{
  class AABB
  {
  public:
    AABB();
    AABB( const Vector3& mini, const Vector3& maxi );
    void Add( const Vector3& point );
    void Add( const AABB& other );
    void Load( const std::vector< Vector3 >& positions );
    static AABB Accumulate( const AABB* aabbs, unsigned numAABBs );
    Vector3 GetCenter() const;
    Vector3 ComputeHalfExtents() const;
    float ComputeVolume() const;
    void Expand( float dist );
    void Get8Points( Vector3* points );
    Vector3 mMin;
    Vector3 mMax;
  };

  class OBB
  {
  public:
    OBB();
    void Load( const std::vector< Vector3 >& positions );
    void Load( const AABB& myAABB );
    float ComputeVolume() const;
    Vector3 mPos;
    Vector3 mHalfExtents;
    Vector3 mAxis[ 3 ];
  };

#define SPHERE_LARSSONx
#define SPHERE_FASTx
#define SPHERE_PCAx

  class BoundingSphere
  {
  public:
    BoundingSphere();
    BoundingSphere( float rad, Vector3 center );
    BoundingSphere( 
      const Vector3& p0 );
    BoundingSphere( 
      const Vector3& p0, 
      const Vector3& p1 );
    BoundingSphere( 
      const Vector3& p0, 
      const Vector3& p1, 
      const Vector3& p2 );
    BoundingSphere( 
      const Vector3& p0, 
      const Vector3& p1, 
      const Vector3& p2, 
      const Vector3& p3 );
    bool Intersects( const BoundingSphere& other ) const;
    bool Contains( const BoundingSphere& other ) const ;
    bool Contains( const Vector3& point ) const ;
#ifdef SPHERE_FAST
    void LoadFast( const std::vector< Vector3 >& positions );
#endif
    void LoadRitter( const std::vector< Vector3 >& positions );
    void LoadCentroid( const std::vector< Vector3 >& positions );
#ifdef SPHERE_PCA
    void LoadPCA( const OBB& box );
#endif
#ifdef SPHERE_LARSSON
    void LoadLarsson(  const std::vector< Vector3 >& positions );
#endif
    void Add( const BoundingSphere & rhs );
    void Add( const Vector3 & rhs );
    void Expand( float dist );
    static BoundingSphere Accumulate(
      const BoundingSphere* spheres, unsigned numSpheres );
    Vector3 GetCenter() const;
    float ComputeVolume() const;
    float mRadius;
    Vector3 mPos;
  };

  class BoundingEllipsoid
  {
  public:
    BoundingEllipsoid();
    void Load( const std::vector< Vector3 >& positions );
    void Load( const OBB& myOBB,
      const std::vector< Vector3 >& positions );
    Vector3 mPos;
    Vector3 mHalfExtents;
    Vector3 mAxis[ 3 ];
  };


  class Shape
  {
  public:
    //virtual Vector3 GetPointOnEdge() const = 0;
    virtual Vector3 Support( const Vector3& dir ) const = 0;
  };

  class PolygonShape : public Shape
  {
  public:
    //Vector3 GetPointOnEdge() const override;
    Vector3 Support( const Vector3& dir ) const override;
    std::vector< Vector3 > mPoints;
  };

  class SphereShape : public Shape
  {
  public:
    SphereShape();
    SphereShape( const BoundingSphere& myBoudningSphere );
    SphereShape( float rad, Vector3 pos );
    Vector3 Support( const Vector3& dir ) const override;
    float mRadius;
    Vector3 mPos;
  };

  class GJK
  {
  public:
    enum GJKResult
    {
      eNotColliding,
      eColliding,
      eNeedsMoreSteps,
    };
    void Init();
    GJKResult Step( const Shape& lhs, const Shape& rhs );
    GJKResult Step( const Shape& minkowskiDiff );
    Vector3 v;
    Vector3 a, b, c, d, e;
    unsigned n;
    unsigned iterCount;
    bool Update( const Vector3& a );
  };

} // Tac

