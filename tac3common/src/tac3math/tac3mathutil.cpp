#include "tac3mathutil.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>

namespace Tac
{
  float Lerp(float begin, float end, float t)
  {
    return (1.0f - t) * begin + t * end;
  }
  float ToDegrees(float radians)
  {
    return radians * 180.0f / PI;
  }
  float ToRadians(float degrees)
  {
    return degrees * PI / 180.0f;
  }
  float IsAbout( float val, float otherval )
  {
    return abs( val - otherval ) < EPSILON;
  }
  Vector4 ToColor4( unsigned hex0xrrggbb )
  {
    float r = ( ( hex0xrrggbb >> 16 ) & 0xff ) / 255.0f;
    float g = ( ( hex0xrrggbb >> 8 ) & 0xff ) / 255.0f;
    float b = ( ( hex0xrrggbb >> 0 ) & 0xff ) / 255.0f;
    float a = 1;
    Vector4 result( r, g, b, a );
    return result;
  }

  // 1st line given by P( s ) = P0 + su
  // 2nd line given by Q( t ) = Q0 + tv
  Vector3 CoplanerLineLineIntersection(
    const Vector3& P0, 
    const Vector3& u,
    const Vector3& Q0, 
    const Vector3& v )
  {
    Vector3 normal = u.Cross( v );
    Vector3 vPerp = normal.Cross( v );
    Vector3 w = P0 - Q0;
    // http://geomalgorithms.com/a05-_intersect-1.html
    float denominator = vPerp.Dot( u );
    if( abs( denominator ) < EPSILON )
    {
      // either lines are parallel, or the normals are really small
      // so maybe normalize them
#if _DEBUG
      __debugbreak();
#endif
      // lines are parallel
      assert( false ); // check if parallel before calling this function
    }
    float sIntersection = -vPerp.Dot( w ) / denominator;
    return P0 + u * sIntersection;
  };

  float RandFloat( float f0, float f1 )
  {
    float randPercent = rand() / float( RAND_MAX );
    return f0 + randPercent * ( f1 - f0 );
  }
}
