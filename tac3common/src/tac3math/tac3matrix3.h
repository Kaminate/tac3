#include <iostream> // output operator
#include "Tac3Vector3.h"
namespace Tac
{
  // this matrix can be thought of as an array of 3 row vectors.
  // Since the default OpenGL matrix is column major, when you pass the matrix
  // ie: using glUniformMatrix3f(), pass GL_TRUE to the transpose argument

  struct Matrix3
  {
    float 
      m00, m01, m02,
      m10, m11, m12,
      m20, m21, m22;
    Matrix3();
    Matrix3(
      const Vector3 & col0,
      const Vector3 & col1,
      const Vector3 & col2);
    Matrix3(
      float mm00, float mm01 , float mm02 ,
      float mm10, float mm11 , float mm12 ,
      float mm20, float mm21 , float mm22 );
    float Determinant() const;
    Matrix3 operator+(const Matrix3 & rhs) const;
    Vector3 operator*(const Vector3& rhs) const;
    Matrix3 operator*(const Matrix3& rhs) const;
    Matrix3 operator*( float val ) const;
    Matrix3& operator*=(const Matrix3& rhs);

    void Zero();
    void Identity();
    void Transpose();
    //void Invert();

    float * operator[]( int row );

    const float * operator[]( int row ) const;



    void Print() const;
    // returns a row vector[3]
    //float * operator [](unsigned row);
    //const float * operator [](unsigned row) const;

    static Matrix3 RotRadX(const float rotRad);
    static Matrix3 RotRadY(const float rotRad);
    static Matrix3 RotRadZ(const float rotRad);
    static Matrix3 FromColVectors(
      const Vector3 & v0,
      const Vector3 & v1,
      const Vector3 & v2);

    //Tensor Product (cross product matrix)
    //   vec3 a, b;
    //   Cross(a ,b) == Tensor(a) * b
    static Matrix3 Tensor(const Vector3& vec);
    //Tilda over n
    static Matrix3 Tilda(const Vector3& vec);
    //Rotates a vector to an angle
    static Matrix3 VectorAlignment(
      const Vector3& vector,
      float rad);
  };
}

