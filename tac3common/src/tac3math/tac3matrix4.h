#pragma once
#include <iostream> // output operator
#include "Tac3Vector4.h"

namespace Tac
{
  // Since the default OpenGL matrix is column major
  // during glUniformMatrix4f(), pass GL_TRUE to the transpose argument
  struct Vector3;
  struct Matrix3;
  struct Matrix4
  {
    float 
      m00, m01, m02, m03, // row0
      m10, m11, m12, m13, // row1
      m20, m21, m22, m23, // row2
      m30, m31, m32, m33; // row3

    Matrix4();
    Matrix4(
      float mm00, float mm01, float mm02, float mm03,
      float mm10, float mm11, float mm12, float mm13,
      float mm20, float mm21, float mm22, float mm23,
      float mm30, float mm31, float mm32, float mm33);

    Vector4 operator*(const Vector4& rhs) const;

    Matrix4 operator+(const Matrix4& rhs) const;
    Matrix4 operator*(const Matrix4& rhs) const;
    Matrix4 operator*(float val) const;

    Matrix4& operator*=(const Matrix4& rhs);
    Matrix4& operator*=(float val);
    float * operator[](int row);
    const float * operator[](int row) const;
    float Determinant() const;

    void Zero();
    void Identity();
    void Transpose();
    static Matrix4 Transform(
      const Vector3 & scale,
      const Matrix3 & rotate,
      const Vector3 & translate);
    static Matrix4 Transform(
      const Vector3 & scale,
      const Vector3 & rotate,
      const Vector3 & translate);
    static Matrix4 TransformInv(
      const Vector3 & nonInversedScale,
      const Vector3 & nonInversedRotate,
      const Vector3 & nonInversedTranslate);
    static Matrix4 TransformInv(
      const Vector3 & nonInversedScale,
      const Matrix3 & nonInversedRotate,
      const Vector3 & nonInversedTranslate);

    //Tensor Product
    static Matrix4 Tensor(const Vector4& vec);

    //Tilda over n
    static Matrix4 Tilda(const Vector4& vec);

    //void Invert();
    //void InvertAffine();

    // returns the inverse of the projection matrix
    static Matrix4 InvertProjectionMatrix(const Matrix4 & projMatrix);

    //Rotates a vector to an angle
    static Matrix4 VectorAlignment(
      const Vector4& vector,
      float rads);
    void Print() const;
  };
}

