#pragma once

#include "Tac3Vector3.h"

namespace Tac
{
  struct Vector4
  {
    float x, y, z, w;
    Vector4(); // does not initialize
    Vector4(float xx , float yy , float zz , float ww );
    Vector4(const Vector3 & vec, float ww );
    Vector4 operator-(void) const;  
    Vector4 operator+(const Vector4& rhs) const;
    Vector4 operator-(const Vector4& rhs) const;
    Vector4 operator*(const float rhs) const;
    Vector4 operator/(const float rhs) const;
    Vector4& operator+=(const Vector4& rhs);
    Vector4& operator-=(const Vector4& rhs);
    Vector4& operator*=(const float rhs);
    Vector4& operator/=(const float rhs);
    float & operator[](unsigned index);
    const float & operator [] (unsigned index) const;
    bool operator==(const Vector4& rhs) const;
    bool operator!=(const Vector4& rhs) const;
    void Zero(void);
    Vector3 XYZ();
  };

  template< typename T >
  struct Vector4Template
  {
    Vector4Template(){}
    float & operator[](unsigned i){return &x[i];}
    const float & operator [] (unsigned index) const {return &x[i]};
    T x,y,z,w;
  };
  typedef Vector4Template< unsigned > Vector4uint;
}
