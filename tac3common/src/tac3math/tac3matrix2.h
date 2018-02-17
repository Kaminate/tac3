#include <iostream>
#include "Tac3Vector2.h"
namespace Tac
{
  // this matrix can be thought of as an array of 2 row vectors.
  // Since the default OpenGL matrix is column major, when you pass the matrix
  // ie: using glUniformMatrix3f(), pass GL_TRUE to the transpose argument

  struct Matrix2
  {
    float 
      m00, m01,
      m10, m11;
    Matrix2();
    Matrix2(
      const Vector2 & col0,
      const Vector2 & col1 );
    Matrix2(
      float mm00, float mm01,
      float mm10, float mm11 );
    Matrix2 operator+(const Matrix2 & rhs) const;
    Vector2 operator*(const Vector2& rhs) const;
    Matrix2 operator*(const Matrix2& rhs) const;
    Matrix2 operator*( float val ) const;
    Matrix2& operator*=(const Matrix2& rhs);
    float Determinant() const;

    void Zero();
    void Identity();
    void Transpose();

    float * operator[]( int row );
    const float * operator[]( int row ) const;

    void Print() const;
  };
}

