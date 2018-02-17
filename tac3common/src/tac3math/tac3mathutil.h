#pragma once
//#include <tac3math\tac3vector4.h>
#include "tac3vector4.h"

namespace Tac
{
#define PI 3.14159265358979323846264f
#define EPSILON 0.00001f

  float Lerp( float begin, float end, float t );
  float ToDegrees( float radians );
  float ToRadians( float degrees );
  float IsAbout( float val, float otherval );

  Vector4 ToColor4( unsigned hex0xrrggbb );
  template< typename T >
  T Clamp( T val, T mini, T maxi )
  {
    if( val < mini )
      val = mini;
    else if( val > maxi )
      val = maxi;
    return val;
  }

  // 1st line given by P( s ) = P0 + su
  // 2nd line given by Q( t ) = Q0 + tv
  Vector3 CoplanerLineLineIntersection(
    const Vector3& P0, 
    const Vector3& u,
    const Vector3& Q0, 
    const Vector3& v );

  float RandFloat( float f0, float f1 );

}
