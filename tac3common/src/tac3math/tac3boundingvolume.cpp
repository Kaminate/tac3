#include "tac3boundingvolume.h"
#include "tac3matrix3.h"
#include "tac3matrix4.h"
#include "tac3mathutil.h"
#include "tac3util\tac3utilities.h"
#include <algorithm> // std::max
#include <assert.h>
#include "eig3.h"

namespace Tac
{
  static Vector3 GetFarthestPoint (
    const Vector3& pos,
    const std::vector< Vector3 >& positions )
  {
    Vector3 farthestPoint = pos;
    float largestDistSq = 0;

    for( const Vector3& curPos : positions )
    {
      float curDistSq = curPos.DistSq( pos );
      if( curDistSq > largestDistSq )
      {
        farthestPoint = curPos;
        largestDistSq = curDistSq;
      }
    }

    return farthestPoint;
  }

  static Vector3 GetAveragePoint(
    const std::vector< Vector3 >& positions )
  {
    assert( !positions.empty() );
    Vector3 sum( 0, 0, 0 );
    for( const Vector3& curPos : positions )
      sum += curPos;
    return sum / float( positions.size() );
  }

  //float GetRunningAverage( float average, unsigned n, float newElement )
  //{
  //  float result = ( average * n + newElement ) / ( n + 1 );
  //  return result;
  //}

  //static Vector3 GetRunningAveragePoint(
  //  const std::vector< Vector3 >& positions )
  //{
  //  assert( !positions.empty() );
  //  Vector3 runningAverage( 0, 0, 0);
  //  for( unsigned iPos = 0; iPos < positions.size(); ++iPos )
  //  {
  //    const Vector3& curPos = positions[ iPos ];
  //    for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
  //    {
  //      float& average = runningAverage[ iAxis ];
  //      average = GetRunningAverage( average, iPos, curPos[ iAxis ] );
  //    }
  //  }
  //  return runningAverage;
  //}

  BoundingSphere::BoundingSphere()
    : mPos(0,0,0)
    , mRadius(0)
  {

  }
  BoundingSphere::BoundingSphere( float rad, Vector3 center )
    : mPos(center), mRadius(rad)
  {

  }
  BoundingSphere::BoundingSphere(
    const Vector3& p0 )
  {
    mPos = p0;
    mRadius = 0;
  }
  BoundingSphere::BoundingSphere(
    const Vector3& p0,
    const Vector3& p1 )
  {
    mPos = ( p0 + p1 ) / 2.0f;
    mRadius = ( p0 - p1 ).Length() / 2.0f;
  }
  BoundingSphere::BoundingSphere(
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& p2 )
  {

    // if they are coplaner
    // get the tangents(t1, t2) of two edges (e1, e2)
    // those 3 lines intersect at the circumcenter c
    //   p2
    //   |\
    //  /|\\
    // e2|  \
    //   |   \
    //   |    \
    //   |  t2 \
    // m2|____\_\c
    //   |    / |\
    //   |      | \
    //   |      |  \
    //   |      t1  \
    //   |     /|\   \
    //   |      |     \
    //   |______|____\_\p1
    //   p0    m1    /e1

    Vector3 e1 = p1 - p0;
    //e1.Normalize();
    Vector3 e2 = p2 - p0;
    //e2.Normalize();
    Vector3 n = e1.Cross( e2 );
    //n.Normalize();
    Vector3 t1 = n.Cross( e1 );
    t1.Normalize();
    Vector3 t2 = e2.Cross( n );
    t2.Normalize();

    Vector3 m1 = ( p0 + p1 ) / 2.0f;
    Vector3 m2 = ( p0 + p2 ) / 2.0f;

    Vector3 intersectionPoint = CoplanerLineLineIntersection(
      m1,
      t1,
      m2,
      t2 );
    mPos = intersectionPoint;
    mRadius = intersectionPoint.Dist( p0 );
  }
  BoundingSphere::BoundingSphere(
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& p2,
    const Vector3& p3 )
  {
    // http://mathworld.wolfram.com/Circumsphere.html
    float p0lenSq = p0.LengthSq();
    float p1lenSq = p1.LengthSq();
    float p2lenSq = p2.LengthSq();
    float p3lenSq = p3.LengthSq();

    Matrix4 matX(
      p0lenSq, p0.y, p0.z, 1,
      p1lenSq, p1.y, p1.z, 1,
      p2lenSq, p2.y, p2.z, 1,
      p3lenSq, p3.y, p3.z, 1 );

    Matrix4 matY(
      p0lenSq, p0.x, p0.z, 1,
      p1lenSq, p1.x, p1.z, 1,
      p2lenSq, p2.x, p2.z, 1,
      p3lenSq, p3.x, p3.z, 1 );

    Matrix4 matZ(
      p0lenSq, p0.x, p0.y, 1,
      p1lenSq, p1.x, p1.y, 1,
      p2lenSq, p2.x, p2.y, 1,
      p3lenSq, p3.x, p3.y, 1 );
    float Dx = matX.Determinant();
    float Dy = -matY.Determinant();
    float Dz = matZ.Determinant();
    float a = Matrix4(
      p0.x, p0.y, p0.z, 1,
      p1.x, p1.y, p1.z, 1,
      p2.x, p2.y, p2.z, 1,
      p3.x, p3.y, p3.z, 1 ).Determinant();
    float c = Matrix4(
      p0lenSq, p0.x, p0.y, p0.z,
      p1lenSq, p1.x, p1.y, p1.z,
      p2lenSq, p2.x, p2.y, p2.z,
      p3lenSq, p3.x, p3.y, p3.z ).Determinant();

    float two_a = 2.0f * a;
    mPos.x = Dx / two_a;
    mPos.y = Dy / two_a;
    mPos.z = Dz / two_a;
    mRadius = sqrt( Dx*Dx + Dy*Dy + Dz*Dz - 4*a*c ) / abs( two_a );
  }
  void BoundingSphere::Add(const BoundingSphere & rhs )
  {
    Vector3 toRhs = rhs.mPos - mPos;
    float distToRhs = toRhs.Length();

    if( Contains( rhs ) )
    {
      // do nothing, we already contain rhs
    }
    else if( rhs.Contains( *this ) )
    {
      *this = rhs;
    }
    else
    {
      Vector3 axis = toRhs;
      axis.Normalize();

      Vector3 point1 = mPos - axis * mRadius;

      float radius2 = std::max(mRadius, rhs.mRadius + distToRhs);
      Vector3 point2 = mPos + axis * radius2;

      mPos = (point1 + point2) / 2.0f;
      mRadius = point1.Dist(point2) / 2.0f;
    }
  }
  void BoundingSphere::Add( const Vector3 & rhs )
  {
    //float dist = mPos.Dist(rhs);
    //float distFromRing = dist - mRadius;
    //if (distFromRing > 0)
    Vector3 topoint = rhs - mPos;
    float distSq = topoint.LengthSq();
    if( distSq > mRadius * mRadius )
    {
      float newRadius = (sqrt(distSq) + mRadius) / 2.0f;
      topoint.Normalize();
      topoint *= newRadius - mRadius;
      mPos += topoint;
      mRadius = newRadius;


      // they are outside
      //Vector3 toNewPoint = rhs - mPos;

      //float halfWay = distFromRing / 2.0f;
      //// move half the way towards them
      //toNewPoint.Normalize();
      //toNewPoint *= halfWay;
      //mPos += toNewPoint;

      //// expand our radius by that much on each side
      //mRadius += halfWay;
    }
    else
    {
      // do nothing, we already bound them
    }
  }
#ifdef SPEHRE_FAST
  void BoundingSphere::LoadFast( 
    const std::vector< Vector3 >& positions )
  {
    unsigned numVerts = positions.size();
    if( numVerts != 0 )
    {
      mPos = positions[ 0 ];
      mRadius = 0;
      for (unsigned i = 1; i < numVerts; ++i)
      {
        Add( positions[ i ] );
      }
    }
  }
#endif
  void BoundingSphere::LoadCentroid( 
    const std::vector< Vector3 >& positions )
  {
    // TLDR( N8 ): bound sphere center = average (centroid) 
    //             bound sphere radius = dist( center, farthest point )
    if( !positions.empty() )
    {
      mPos = GetAveragePoint( positions );
      Vector3 farthestPoint = GetFarthestPoint( mPos, positions );
      mRadius = mPos.Dist( farthestPoint );
    }

#if 0
    float maxX = positions[ 0 ].x;
    float minX = positions[ 0 ].x;
    float avgX = 0;

    for( const Vector3& v : positions )
    {
      if ( v.x > maxX ) maxX = v.x;
      if ( v.x < minX ) minX = v.x;
      avgX += v.x;
    }
    avgX /= float( positions.size() );
    float xRadius = std::max( maxX - avgX, avgX - minX );
    std::cout << minX;
    std::cout << maxX;
    std::cout << avgX;
    std::cout << xRadius;
#endif

  }
  void BoundingSphere::LoadRitter( 
    const std::vector< Vector3 >& positions )
  {
    // gets extrema points on a random axis to build the initial sphere
    // then grows from that
    unsigned numVerts = positions.size();
    if( numVerts != 0 )
    {
      Vector3 x = positions[ rand() % positions.size() ];
      Vector3 y = GetFarthestPoint( x, positions );
      Vector3 z = GetFarthestPoint( y, positions );
      mPos = ( y + z ) / 2.0f;
      mRadius = y.Dist( x ) / 2.0f;
      for( const Vector3& pos : positions )
        Add( pos );
    }
  }
  BoundingSphere BoundingSphere::Accumulate( 
    const BoundingSphere* spheres, unsigned numSpheres )
  {
#if 1
    BoundingSphere result = spheres[ 0 ];
    for( const BoundingSphere* cursphere = spheres + 1;
      cursphere != spheres + numSpheres;
      cursphere++ )
    {
      result.Add( *cursphere );
    }
    return result;
#else
    BoundingSphere result;
    result.mPos = Vector3( 0, 0, 0 );
    result.mRadius = 0;

    float totalRadius = 0;
    for( const BoundingSphere* cursphere = spheres;
      cursphere != spheres + numSpheres;
      cursphere++ )
    {
      totalRadius += cursphere->mRadius;
    }

    // position is a weighted average ( weighted by radius )
    for( const BoundingSphere* cursphere = spheres;
      cursphere != spheres + numSpheres;
      cursphere++ )
    {
      float scale = ( cursphere->mRadius / totalRadius );
      result.mPos += cursphere->mPos * scale;
    }

    // todo: replace this with max dist from result.mPos 
    // to the furthest point on each sphere
    for( const BoundingSphere* cursphere = spheres;
      cursphere != spheres + numSpheres;
      cursphere++ )
    {
      result.Add( *cursphere );
    }

    return result;
#endif
  }
  void BoundingSphere::Expand( float dist )
  {
    mRadius += dist;
  }
#ifdef SPHERE_PCA
  void BoundingSphere::LoadPCA( const OBB& box )
  {
    mPos = box.mPos;
    mRadius = box.mHalfExtents.Length();
  }
#endif
  static BoundingSphere FlipCodeMiniball(
    Vector3* points,
    unsigned numBoundary, // at front of array
    unsigned numUnsorted )// at back of array
  {
    BoundingSphere result;

    if( numUnsorted == 0 || numBoundary == 4 )
    {
      switch ( numBoundary )
      {
      case 0:
        result.mPos.Zero();
        result.mRadius = 0;
        break;
      case 1:
        result = BoundingSphere( 
          points[ 0 ] );
        break;
      case 2:
        result = BoundingSphere( 
          points[ 0 ],
          points[ 1 ] );
        break;
      case 3:
        result = BoundingSphere( 
          points[ 0 ],
          points[ 1 ],
          points[ 2 ] );
        break;
      case 4:
        result = BoundingSphere( 
          points[ 0 ],
          points[ 1 ],
          points[ 2 ],
          points[ 3 ] );
        break;
      }
    }
    else
    {
      unsigned firstUnsortedIndex = numBoundary; 
      unsigned lastUnsortedIndex = numBoundary + numUnsorted - 1;
      const Vector3& lastElement = points[ lastUnsortedIndex ];
      result = FlipCodeMiniball( 
        points,
        numBoundary,
        numUnsorted - 1 );
      if( !result.Contains( lastElement ) )
      {
        // lastElement is outside the sphere, 
        // so it forms part of the boundary of a bigger sphere
        std::swap( 
          points[ firstUnsortedIndex ],
          points[ lastUnsortedIndex ] );
        result = FlipCodeMiniball( 
          points, 
          numBoundary + 1,
          numUnsorted - 1 );
      }
    }
    return result;
  }

#ifdef SPHERE_LARSSON
  void BoundingSphere::LoadLarsson(
    const std::vector< Vector3 >& positions )
  {
    std::vector< Vector3 > normals001;
    {
      normals001.reserve(3);
      normals001.push_back( Vector3( 1, 0, 0 ) );
      normals001.push_back( Vector3( 0, 1, 0 ) );
      normals001.push_back( Vector3( 0, 0, 1 ) );
    }
    std::vector< Vector3 > normals111;
    {
      normals111.reserve(4);
      normals111.push_back( Vector3( 1, 1, 1) );
      normals111.push_back( Vector3( 1, 1, -1 ) );
      normals111.push_back( Vector3( 1, -1, 1 ) );
      normals111.push_back( Vector3( 1, -1, -1 ) );
      for( Vector3& vec : normals111 )
        vec.Normalize();
    }
    std::vector< Vector3 > normals011;
    {
      normals011.reserve(6);
      normals011.push_back( Vector3(1, 1, 0));
      normals011.push_back( Vector3 (1, -1, 0));
      normals011.push_back( Vector3(1, 0, 1));
      normals011.push_back( Vector3(1, 0, -1));
      normals011.push_back( Vector3(0, 1, 1));
      normals011.push_back( Vector3(0, 1, -1));
      for( Vector3& vec : normals011 )
        vec.Normalize();
    }
    std::vector< Vector3 > normals012;
    {
      normals012.reserve(12);
      normals012.push_back( Vector3( 0, 1, 2));
      normals012.push_back( Vector3(0, 2, 1) );
      normals012.push_back( Vector3(1, 0, 2) );
      normals012.push_back( Vector3(2, 0, 1) );
      normals012.push_back( Vector3(1, 2, 0) );
      normals012.push_back( Vector3(2, 1, 0) );
      normals012.push_back( Vector3(0, 1, -2) );
      normals012.push_back( Vector3(0, 2, -1) );
      normals012.push_back( Vector3(1, 0, -2) );
      normals012.push_back( Vector3(2, 0, -1) );
      normals012.push_back( Vector3(1, -2, 0) );
      normals012.push_back( Vector3(2, -1, 0) );
      for( Vector3& vec : normals012 )
        vec.Normalize();
    }
    std::vector< Vector3 > normals112;
    {
      normals112.reserve(12);
      normals112.push_back( Vector3(1, 1, 2) );
      normals112.push_back( Vector3 (2, 1, 1) );
      normals112.push_back( Vector3(1, 2, 1) );
      normals112.push_back( Vector3(1, -1, 2) );
      normals112.push_back( Vector3(1, 1, -2) );
      normals112.push_back( Vector3(1, -1, -2) );
      normals112.push_back( Vector3(2, -1, 1) );
      normals112.push_back( Vector3(2, 1, -1) );
      normals112.push_back( Vector3(2, -1, -1) );
      normals112.push_back( Vector3(1, -2, 1) );
      normals112.push_back( Vector3(1, 2, -1) );
      normals112.push_back( Vector3(1, -2, -1) );
      for( Vector3& vec : normals112 )
        vec.Normalize();
    }
    std::vector< Vector3 > normals122;
    {
      normals122.reserve(12);
      normals122.push_back( Vector3(2, 2, 1) );
      normals122.push_back( Vector3 (1, 2, 2) );
      normals122.push_back( Vector3(2, 1, 2) );
      normals122.push_back( Vector3(2, -2, 1) );
      normals122.push_back( Vector3(2, 2, -1) );
      normals122.push_back( Vector3(2, -2, -1) );
      normals122.push_back( Vector3(1, -2, 2) );
      normals122.push_back( Vector3(1, 2, -2) );
      normals122.push_back( Vector3(1, -2, -2) );
      normals122.push_back( Vector3(2, -1, 2) );
      normals122.push_back( Vector3(2, 1, -2) );
      normals122.push_back( Vector3(2, -1, -2) );
      for( Vector3& vec : normals122 )
        vec.Normalize();
    }

    // problems with normals001 where in a cube, the (1,1,1)
    // be the largest in x, y and z and will repeat 3 times.
    // 
    // can either solve with code, or hack by choosing normals 012 instead
    const std::vector< Vector3 >& normals = normals012;
    std::vector< Vector3 > extremePoints;
    for( const Vector3& normal : normals )
    {
      unsigned largestProjIndex = 0;
      unsigned smallestProjIndex = 0;
      float largestProjDist, smallestProjDist;
      largestProjDist = smallestProjDist = normal.Dot( positions[ 0 ] );
      for( unsigned iPos = 1; iPos < positions.size(); ++iPos )
      {
        float dist = normal.Dot( positions[ iPos ] );
        if( dist > largestProjDist )
        {
          largestProjDist = dist;
          largestProjIndex = iPos;
        }
        else if( dist < smallestProjDist )
        {
          smallestProjDist = dist;
          smallestProjIndex = iPos;
        }
      }
      extremePoints.push_back( positions[ largestProjIndex ] );
      extremePoints.push_back( positions[ smallestProjIndex ] );
    }

    *this = FlipCodeMiniball( 
      &extremePoints.front(),
      0,
      extremePoints.size() );

    for( const Vector3& pos : positions )
    {
      Add( pos );
    }
  }
#endif
  bool BoundingSphere::Intersects( 
    const BoundingSphere& other ) const
  {
    // expand ourselves by other's radius,
    // shrink other to a point
    // see if our expanded sphere contains ther other point
    BoundingSphere expandedSphere = *this;
    expandedSphere.Expand( other.mRadius );
    return expandedSphere.Contains( other.mPos );
  }
  bool BoundingSphere::Contains( const BoundingSphere& other ) const
  {
    bool result = false;
    float radiusDiff = mRadius - other.mRadius;
    if( radiusDiff > 0 )
      result = mPos.Dist( other.mPos ) < radiusDiff;
    return result;
  }
  bool BoundingSphere::Contains( const Vector3& point ) const
  {
    return mPos.DistSq( point ) < ( mRadius * mRadius );
  }
  float BoundingSphere::ComputeVolume() const
  {
    return ( 4.0f / 3.0f ) * PI * mRadius * mRadius * mRadius;
  }
  Vector3 BoundingSphere::GetCenter() const
  {
    return mPos;
  }

  AABB::AABB()
  {

  }
  AABB::AABB( const Vector3& mini, const Vector3& maxi ) :
    mMin( mini ),
    mMax( maxi )
  {

  }
  void AABB::Add( const Vector3& point )
  {
    for( unsigned i = 0; i < 3; ++i )
    {
      if( point[ i ] < mMin[ i ] )
        mMin[ i ] = point[ i ];
      else if( point[ i ] > mMax[ i ] )
        mMax[ i ] = point[ i ];
    }
  }
  void AABB::Add( const AABB& other )
  {
    for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
    {
      mMin[ iAxis ] = std::min( mMin[ iAxis ], other.mMin[ iAxis ] );
      mMax[ iAxis ] = std::max( mMax[ iAxis ], other.mMax[ iAxis ] );
    }
  }
  void AABB::Load( const std::vector< Vector3 >& positions )
  {
    unsigned posCount = positions.size();
    if( posCount != 0 )
    {
      mMin = mMax = positions.front();
      for( unsigned i = 1; i < posCount; ++i )
      {
        Add( positions[ i ] );
      }
    }
    // expand very slightly for zfighting
    mMin -= Vector3( 0.01f, 0.01f, 0.01f );
    mMax += Vector3( 0.01f, 0.01f, 0.01f );
  }
  Vector3 AABB::ComputeHalfExtents() const
  {
    Vector3 result = ( mMax - mMin ) / 2.0f;
    return result;
  }
  AABB AABB::Accumulate( const AABB* aabbs, unsigned numAABBs )
  {
    AABB result = aabbs[ 0 ];
    for( unsigned i = 0; i < numAABBs; ++i )
    {
      const AABB& curAABB = aabbs[ i ];
      for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
      {
        result.mMin[ iAxis ] = std::min(
          result.mMin[ iAxis ], curAABB.mMin[ iAxis ] );
        result.mMax[ iAxis ] = std::max(
          result.mMax[ iAxis ], curAABB.mMax[ iAxis ] );
      }
    }
    return result;
  }
  void AABB::Expand( float dist )
  {
    Vector3 offset( dist, dist, dist );
    mMin -= offset;
    mMax += offset;
  }
  float AABB::ComputeVolume() const
  {
    Vector3 diff = mMax - mMin;
    return diff.x * diff.y * diff.z;
  }
  Vector3 AABB::GetCenter() const
  {
    return ( mMin + mMax ) / 2.0f;
  }

  void AABB::Get8Points( Vector3* points )
  {
    points[ 0 ] = mMin;
    points[ 1 ] = mMin; points[ 1 ].x = mMax.x;
    points[ 2 ] = mMin; points[ 2 ].y = mMax.y;
    points[ 3 ] = mMin; points[ 3 ].z = mMax.z;

    points[ 4 ] = mMax;
    points[ 5 ] = mMax; points[ 5 ].x = mMin.x;
    points[ 6 ] = mMax; points[ 6 ].y = mMin.y;
    points[ 7 ] = mMax; points[ 7 ].z = mMin.z;
  }

  static void Decompose(
    const std::vector< Vector3 >& positions,
    Vector3 axis[ 3 ],
    Vector3& halfExtents )
  {
    assert( !positions.empty() );

    Vector3 averagePoint = GetAveragePoint( positions );

    auto Covariance = [](
      const Vector3& averagePos,
      unsigned axis0,
      unsigned axis1,
      const std::vector< Vector3 >& positions )
    {
      float sum = 0;
      for( const Vector3& pos : positions )
      {
        sum 
          += ( pos[ axis0 ] - averagePos[ axis0 ] ) 
          * ( pos[ axis1 ] - averagePos[ axis1 ] );
      }
      float result = sum / positions.size();
      return result;
    };
    float xx = Covariance( averagePoint, 0, 0, positions );
    float xy = Covariance( averagePoint, 0, 1, positions );
    float xz = Covariance( averagePoint, 0, 2, positions );;
    float yy = Covariance( averagePoint, 1, 1, positions );;
    float yz = Covariance( averagePoint, 1, 2, positions );;
    float zz = Covariance( averagePoint, 2, 2, positions );;

    double A[ 3 ][ 3 ];
    A[ 0 ][ 0 ] = xx;
    A[ 0 ][ 1 ] = xy;
    A[ 0 ][ 2 ] = xz;

    A[ 1 ][ 0 ] = xy;
    A[ 1 ][ 1 ] = yy;
    A[ 1 ][ 2 ] = yz;

    A[ 2 ][ 0 ] = xz;
    A[ 2 ][ 1 ] = yz;
    A[ 2 ][ 2 ] = zz;

    double V[ 3 ][ 3 ];
    double d[ 3 ];
    eigen_decomposition( A, V, d ); 

    for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
    {
      axis[ iAxis ] = Vector3(
        ( float )V[ 0 ][ iAxis ],
        ( float )V[ 1 ][ iAxis ],
        ( float )V[ 2 ][ iAxis ] );
      axis[ iAxis ].Normalize();
    }

    // keep handedness!
    if ( axis[ 0 ].Cross( axis[ 1 ] ).Dot( axis[ 2 ] ) < 0 )
    {
      axis[ 2 ] *= -1.0f;
    }
  }

  OBB::OBB()
  {

  }
  void OBB::Load( const std::vector< Vector3 >& positions )
  {
    Decompose( positions, mAxis, mHalfExtents );

    Vector3 mins;
    for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
    {
      mins[ iAxis ] = mAxis[ iAxis ].Dot( positions[ iAxis ] );
    }
    Vector3 maxs = mins;

    for( unsigned iPos = 1; iPos < positions.size(); ++iPos )
    {
      const Vector3& pos = positions[ iPos ];
      for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
      {
        float projectedDist = mAxis[ iAxis ].Dot( pos );
        if( projectedDist < mins[ iAxis ] )
          mins[ iAxis ] = projectedDist;
        else if( projectedDist > maxs[ iAxis ] )
          maxs[ iAxis ] = projectedDist;
      }
    }

    mPos.Zero();
    for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
    {
      mPos += mAxis[ iAxis ] * ( mins[ iAxis ] + maxs[ iAxis ] ) / 2.0f;
      mHalfExtents[ iAxis ] = ( maxs[ iAxis ] - mins[ iAxis ] ) / 2.0f;
    }



    // also try the aabb
    AABB testAABB;
    testAABB.Load( positions );

    if( testAABB.ComputeVolume() < ComputeVolume() )
    {
      Load( testAABB );
    }
  }
  void OBB::Load( const AABB& myAABB )
  {
    mPos = myAABB.GetCenter();
    mAxis[ 0 ] = Vector3( 1, 0, 0 );
    mAxis[ 1 ] = Vector3( 0, 1, 0 );
    mAxis[ 2 ] = Vector3( 0, 0, 1 );
    mHalfExtents = ( myAABB.mMax - myAABB.mMin ) / 2.0f;
  }
  float OBB::ComputeVolume() const
  {
    float result 
      = mHalfExtents.x * mHalfExtents.y * mHalfExtents.z * 8.0f;
    return result;
  }

  BoundingEllipsoid::BoundingEllipsoid()
  {

  }
  void BoundingEllipsoid::Load( const std::vector< Vector3 >& positions )
  {
    OBB myOBB;
    myOBB.Load( positions );
    Load( myOBB, positions );
  }
  void BoundingEllipsoid::Load(
    const OBB& myOBB,
    const std::vector< Vector3 >& positions )
  {
    mPos = myOBB.mPos;
    mHalfExtents = myOBB.mHalfExtents;
    memcpy( mAxis, myOBB.mAxis, sizeof( mAxis ) );

    // from column vectors
    Matrix3 toEllipsoid( mAxis[ 0 ], mAxis[ 1 ], mAxis[ 2 ] ); 
    toEllipsoid.Transpose();

    for( const Vector3& pos : positions )
    {
      Vector3 p = toEllipsoid * ( pos - mPos );
      float sum = 0;
      for( unsigned iAxis = 0; iAxis < 3; ++iAxis )
      {
        float part = p[ iAxis ] / mHalfExtents[ iAxis ];
        sum += part * part;
      }

      if( sum > 1.0f ) // pos is outside the ellipsoid
      {
        // scale the ellipsoid
        mHalfExtents *= sqrt( sum );
      }
    }

  }

  //Vector3 PolygonShape::GetPointOnEdge() const
  //{
  //  assert( !mPoints.empty() );
  //  return mPoints[ 0 ];
  //}

  Vector3 PolygonShape::Support( const Vector3& dir ) const
  {
    assert( !mPoints.empty() );
    unsigned mostDotIndex = 0;
    float mostDot = mPoints[ mostDotIndex ].Dot( dir );
    for( unsigned i = 1; i < mPoints.size(); ++i )
    {
      float currDot = mPoints[ i ].Dot( dir );
      if( currDot > mostDot )
      {
        mostDot = currDot;
        mostDotIndex = i;
      }
    }
    Vector3 result = mPoints[ mostDotIndex ];
    return result;
  }


  void GJK::Init()
  {
    b = Vector3( 0, 0, 0 );
    c = Vector3( 0, 0, 0 );
    d = Vector3( 0, 0, 0 );
    v = Vector3( 1, 0, 0 );
    n = 0;
    iterCount = 0;
  }


  GJK::GJKResult GJK::Step( const Shape& minkowskiDiff )
  {
    ++iterCount;

    a = minkowskiDiff.Support( v );

    if( a.Dot( v ) < 0 )
      return eNotColliding;

    if( Update( a ) )
      return eColliding;

    if( iterCount == 20 )
      return eColliding;

    return eNeedsMoreSteps;
  
  }
  GJK::GJKResult GJK::Step( const Shape& lhs, const Shape& rhs )
  {
    ++iterCount;

    Vector3 lhsSupport = lhs.Support( v );
    Vector3 rhsSupport = rhs.Support( -v );
    a = lhsSupport - rhsSupport;

    if( a.Dot( v ) < 0 )
      return eNotColliding;

    if( Update( a ) )
      return eColliding;

    if( iterCount == 20 )
      return eColliding;

    return eNeedsMoreSteps;
  }

  bool GJK::Update( const Vector3& a )
  {
    switch ( n )
    {
    case 0:
      {
        b = a;
        v = -a;
        n = 1;
        return false;
      } break;
    case 1:
      {
        Vector3 btoa = a - b;

        // divide by 0?
        btoa.Normalize();

        Vector3 vPara = btoa * v.Dot( btoa );
        v = v - vPara;

        c = b;
        b = a;

        n = 2;
        return false;
      } break;
    case 2:
      {
        // our simplex is currently the line segment bc
        // we know the origin is in the direction of v away from out line
        // segment

        // a b and c form a triangle

        // edge vectors
        Vector3 ab = b - a;
        Vector3 ac = c - a;

        // triangle normal
        Vector3 abc = ab.Cross( ac );

        // for clarity
        const Vector3 ao = -a;

        // vector facing away from edge ab
        Vector3 abp = ab.Cross( abc );
        if( abp.Dot( ao ) > 0 )
        {
          // origin lies outside triangle on the side of the edge ab
          c = b;
          b = a;
          ab.Normalize();
          Vector3 para = ab * abp.Dot( ab );
          v = abp - para;
          return false;
        }

        // same test for edge ac
        Vector3 acp = abc.Cross( ac );
        if( acp.Dot( ao ) > 0 )
        {
          // origin lies outside triangle on the side of the edge ab
          b = a;
          ac.Normalize();
          Vector3 para = ac * acp.Dot( ac );
          v = acp - para;
          return false;
        }

        // the origin can't be on the outside of the triangle on the
        // side of the edge bc, because v points the other direction
        //
        // so instead we must determine if the origin is above or below us
        if( ao.Dot( abc ) > 0 )
        {
          d = c;
          c = b;
          b = a;

          v = abc;
        }
        else
        {
          // reverse winding order
          d = b;
          b = a;

          v = -abc;
        }
        n = 3;
        return false;
      } break;
    case 3:
      {
        Vector3 ao = -a;
        Vector3 ab = b - a;
        Vector3 ac = c - a;
        Vector3 abc = ab.Cross( ac );

        {

          if( abc.Dot( ao ) > 0 )
          {
            // in front of ABC
            goto checkface;
          }

          Vector3 ad = d - a;
          Vector3 acd = ac.Cross( ad );
          if( acd.Dot( ao ) > 0 )
          {
            // in front of ACD
            b = c;
            c = d;
            ab = ac;
            ac = ad;
            abc = acd;
            // in front of ABC
            goto checkface;
          }

          Vector3 adb = ad.Cross( ab );
          if( adb.Dot( ao ) > 0 )
          {
            // in front of ADB
            c = b;
            b = d;
            ac = ab;
            ab = ad;
            abc = adb;
            // in front of ABC
            goto checkface;
          }

        }
        // behind all 3 faces, origin is in the tetrahedron abcd
        e = d;
        d = c;
        c = b;
        b = a;

        n = 4;
        return true;

checkface:

        // triangle is ccw around ABC
        // origin is in direction abc

        Vector3 abp = ab.Cross( abc );
        if( ao.Dot( abp ) > 0 )
        {
          // NOTE( N8 ): basically dropped 2 points
          c = b;
          b = a;
          ab.Normalize();
          Vector3 para = ab * ao.Dot( ab );
          v = ao - para;
          n = 2;
          return false;
        }

        Vector3 acp = abc.Cross( ac );
        if( ao.Dot( acp ) > 0 )
        {
          // NOTE( N8 ): basically dropped 2 points
          b = a;
          ac.Normalize();
          Vector3 para = ac * ac.Dot( ao );
          v = ao - para;
          n = 2;
          return false;
        }

        // it's in the triangle
        d = c;
        c = b;
        b = a;
        v = abc;
        n = 3;
        return false;
      } break;
    }


#if _DEBUG
    __debugbreak();
#endif
    assert( false );
    return false;
  }


  SphereShape::SphereShape( const BoundingSphere& myBoudningSphere ) :
    mPos( myBoudningSphere.mPos ),
    mRadius( myBoudningSphere.mRadius )
  {

  }

  SphereShape::SphereShape( float rad, Vector3 pos ) :
    mRadius( rad ),
    mPos( pos )
  {

  }

  SphereShape::SphereShape()
  {

  }

  Vector3 SphereShape::Support( const Vector3& dir ) const 
  {
    Vector3 normalizedDir = dir;
    normalizedDir.Normalize();
    return mPos + normalizedDir * mRadius;
  }

} // Tac
