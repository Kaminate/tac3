#include "tac3matrix4.h"
#include "tac3matrix3.h"
#include <iostream>
#include <iomanip>
namespace Tac
{
  Matrix4::Matrix4(
    float mm00, float mm01, float mm02, float mm03,
    float mm10, float mm11, float mm12, float mm13,
    float mm20, float mm21, float mm22, float mm23,
    float mm30, float mm31, float mm32, float mm33)
    : m00(mm00), m01(mm01), m02(mm02), m03(mm03)
    , m10(mm10), m11(mm11), m12(mm12), m13(mm13)
    , m20(mm20), m21(mm21), m22(mm22), m23(mm23)
    , m30(mm30), m31(mm31), m32(mm32), m33(mm33)
  {
  }

  Matrix4::Matrix4()
  {

  }

  Vector4 Matrix4::operator*(const Vector4& rhs) const
  {
    return Vector4(
      m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + m03 * rhs.w,
      m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + m13 * rhs.w,
      m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + m23 * rhs.w,
      m30 * rhs.x + m31 * rhs.y + m32 * rhs.z + m33 * rhs.w);
  }
  Matrix4 Matrix4::operator+(const Matrix4& rhs) const
  {
    return Matrix4(
      m00 + rhs.m00,
      m01 + rhs.m01,
      m02 + rhs.m02,
      m03 + rhs.m03,
      m10 + rhs.m10,
      m11 + rhs.m11,
      m12 + rhs.m12,
      m13 + rhs.m13,
      m20 + rhs.m20,
      m21 + rhs.m21,
      m22 + rhs.m22,
      m23 + rhs.m23,
      m30 + rhs.m30,
      m31 + rhs.m31,
      m32 + rhs.m32,
      m33 + rhs.m33);
  }
  Matrix4 Matrix4::operator*(const Matrix4& rhs) const
  {
    Matrix4 temp;

    temp.m00 = m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30;
    temp.m01 = m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31;
    temp.m02 = m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32;
    temp.m03 = m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33;

    temp.m10 = m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30;
    temp.m11 = m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31;
    temp.m12 = m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32;
    temp.m13 = m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33;

    temp.m20 = m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30;
    temp.m21 = m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31;
    temp.m22 = m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32;
    temp.m23 = m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33;

    temp.m30 = m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30;
    temp.m31 = m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31;
    temp.m32 = m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32;
    temp.m33 = m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33;

    return temp;
  }

  Matrix4 Matrix4::operator*( float val ) const
  {
    return Matrix4(
      m00 * val, m01 * val, m02 * val, m03 * val,
      m10 * val, m11 * val, m12 * val, m13 * val,
      m20 * val, m21 * val, m22 * val, m23 * val,
      m30 * val, m31 * val, m32 * val, m33 * val);
  }

  Matrix4& Matrix4::operator*=(const Matrix4& rhs)
  {
    *this = *this * rhs;
    return *this;
  }

  Matrix4& Matrix4::operator*=( float val )
  {
    m00 *= val;
    m01 *= val;
    m02 *= val;
    m03 *= val;

    m10 *= val;
    m11 *= val;
    m12 *= val;
    m13 *= val;

    m20 *= val;
    m21 *= val;
    m22 *= val;
    m23 *= val;

    m30 *= val;
    m31 *= val;
    m32 *= val;
    m33 *= val;
    return *this;
  }

  void Matrix4::Zero()
  {
    // probably safe
    memset(this, 0, sizeof(Matrix4));
  }

  void Matrix4::Identity()
  {
    Zero();
    m00 = 1;
    m11 = 1;
    m22 = 1;
    m33 = 1;
  }
  void Matrix4::Transpose()
  {
    Matrix4 temp(
      m00,m10,m20,m30,
      m01,m11,m21,m31,
      m02,m12,m22,m32,
      m03,m13,m23,m33);
    *this = temp;
  }

  Matrix4 Matrix4::Tensor(const Vector4& vec)
  {
    return Matrix4 (
      vec[0] * vec[0], vec[0] * vec[1], vec[0] * vec[2], 0,
      vec[1] * vec[0], vec[1] * vec[1], vec[1] * vec[2], 0,
      vec[2] * vec[0], vec[2] * vec[1], vec[2] * vec[2], 0,
      0,0,0,1);
  }
  Matrix4 Matrix4::Tilda(const Vector4& vec)
  {
    Matrix4 temp(
      0, -vec.z,  vec.y, 0,
      vec.z, 0, -vec.x, 0,
      -vec.y,  vec.x, 0, 0,
      0, 0, 0, 1);

    return temp;
  }
  Matrix4 Matrix4::VectorAlignment(const Vector4& vector,float rads)
  {
    // this is pheonix's funciton, it may not work
    Matrix4 CosI;
    CosI.Identity();
    CosI *= cos(rads);
    CosI.m33 = 1;

    Matrix4 NxN;
    NxN = Tensor(vector);
    NxN *= (1 - cos(rads));
    NxN.m33 = 1;

    Matrix4 SquiglyN;
    SquiglyN = Tilda(vector);
    SquiglyN *= sin(rads);
    SquiglyN.m33 = 1;

    return CosI + NxN + SquiglyN;
  }

  //void Matrix4::Invert()
  //{
  //  if(  abs(v[12] < 0.001f)
  //    && abs(v[13] < 0.001f)
  //    && abs(v[14] < 0.001f)
  //    && abs(v[15] - 1) < 0.001f)
  //    InvertAffine();
  //  else
  //    Identity();
  //}

  //void Matrix4::InvertAffine()
  //{
  //  Matrix3 r(v[0],v[1],v[2], v[4],v[5],v[6], v[8],v[9],v[10]);
  //  r.Invert();
  //  v[0] = r.v[0];  v[1] = r.v[1];  v[2] = r.v[2];
  //  v[4] = r.v[3];  v[5] = r.v[4];  v[6] = r.v[5];
  //  v[8] = r.v[6];  v[9] = r.v[7];  v[10]= r.v[8];
  //  // -R^-1 * T
  //  float x = v[3];
  //  float y = v[7];
  //  float z = v[11];
  //  v[3]  = -(r.v[0] * x + r.v[1] * y + r.v[2] * z);
  //  v[7]  = -(r.v[3] * x + r.v[4] * y + r.v[5] * z);
  //  v[11] = -(r.v[6] * x + r.v[7] * y + r.v[8] * z);
  //}


  Tac::Matrix4 Matrix4::InvertProjectionMatrix( const Matrix4 & projMatrix )
  {
    return Matrix4(
      1/projMatrix.m00, 0, 0, 0,
      0, 1/projMatrix.m11, 0, 0,
      0,0,0,-1,
      0,0,1/projMatrix.m23, projMatrix.m22/projMatrix.m23);
  }

  Tac::Matrix4 Matrix4::Transform( 
    const Vector3 & scale, 
    const Matrix3 & rot, 
    const Vector3 & translate )
  {
    return Matrix4(
      scale.x * rot.m00, scale.y * rot.m01, scale.z * rot.m02, translate.x,
      scale.x * rot.m10, scale.y * rot.m11, scale.z * rot.m12, translate.y,
      scale.x * rot.m20, scale.y * rot.m21, scale.z * rot.m22, translate.z,
      0, 0, 0, 1);
  }
  Tac::Matrix4 Matrix4::Transform( 
    const Vector3 & scale, 
    const Vector3 & rotate, 
    const Vector3 & translate )
  {
    if(rotate == Vector3(0,0,0))
    {
      return Matrix4(
        scale.x, 0, 0, translate.x,
        0, scale.y, 0, translate.y,
        0, 0, scale.z, translate.z,
        0, 0, 0, 1);
    }
    else
    {
      Matrix3 rot 
        = Matrix3::RotRadZ(rotate.z)
        * Matrix3::RotRadY(rotate.y)
        * Matrix3::RotRadX(rotate.x);
      return Matrix4(
        scale.x * rot.m00, scale.y * rot.m01, scale.z * rot.m02, translate.x,
        scale.x * rot.m10, scale.y * rot.m11, scale.z * rot.m12, translate.y,
        scale.x * rot.m20, scale.y * rot.m21, scale.z * rot.m22, translate.z,
        0, 0, 0, 1);
    }
  }

  Tac::Matrix4 Matrix4::TransformInv( 
    const Vector3 & nonInversedScale,
    const Vector3 & nonInversedRotate,
    const Vector3 & nonInversedTranslate )
  {
    Vector3 s(
      1/nonInversedScale.x, 
      1/nonInversedScale.y, 
      1/nonInversedScale.z);
    // NOTE( N8 ): this MUST be opposite order of Matrix4::Transform
    // ( transform goes zyx, so we go xyz )
    Matrix3 r
      = Matrix3::RotRadX(-nonInversedRotate.x)
      * Matrix3::RotRadY(-nonInversedRotate.y)
      * Matrix3::RotRadZ(-nonInversedRotate.z);
    Vector3 t(
      -nonInversedTranslate.x,
      -nonInversedTranslate.y,
      -nonInversedTranslate.z);

    float m03 = s.x * ( t.x * r.m00 + t.y * r.m01 + t.z * r.m02 );
    float m13 = s.y * ( t.x * r.m10 + t.y * r.m11 + t.z * r.m12 );
    float m23 = s.z * ( t.x * r.m20 + t.y * r.m21 + t.z * r.m22 );

    return Matrix4(
      s.x * r.m00, s.x * r.m01, s.x * r.m02, m03,
      s.y * r.m10, s.y * r.m11, s.y * r.m12, m13,
      s.z * r.m20, s.z * r.m21, s.z * r.m22, m23,
      0, 0, 0, 1);
  }

  Tac::Matrix4 Matrix4::TransformInv( 
    const Vector3 & nonInversedScale,
    const Matrix3 & nonInversedRotate,
    const Vector3 & nonInversedTranslate )
  {
    Vector3 s(
      1/nonInversedScale.x, 
      1/nonInversedScale.y, 
      1/nonInversedScale.z);
    Matrix3 r = nonInversedRotate;
    r.Transpose();
    Vector3 t(
      -nonInversedTranslate.x,
      -nonInversedTranslate.y,
      -nonInversedTranslate.z);

    float m03 = s.x * ( t.x * r.m00 + t.y * r.m01 + t.z * r.m02 );
    float m13 = s.y * ( t.x * r.m10 + t.y * r.m11 + t.z * r.m12 );
    float m23 = s.z * ( t.x * r.m20 + t.y * r.m21 + t.z * r.m22 );

    return Matrix4(
      s.x * r.m00, s.x * r.m01, s.x * r.m02, m03,
      s.y * r.m10, s.y * r.m11, s.y * r.m12, m13,
      s.z * r.m20, s.z * r.m21, s.z * r.m22, m23,
      0, 0, 0, 1);
  }
  void Matrix4::Print() const
  {
    for(int row = 0; row < 4; ++row)
    {
      for(int col  = 0; col < 4; ++col)
      {
        std::cout 
          << std::setprecision(2) 
          << std::fixed 
          << std::setw(6) <<
          this->operator[](row)[col] 
        << " ";
      }
      std::cout << std::endl;
    }
  }

  float * Matrix4::operator[]( int row )
  {
    return &m00 + row * 4;
  }

  const float * Matrix4::operator[]( int row ) const
  {
    return &m00 + row * 4;
  }

  float Matrix4::Determinant() const
  {
    m00, m01, m02, m03;
    m10, m11, m12, m13;
    m20, m21, m22, m23;
    m30, m31, m32, m33;

    return 
      m00 * Matrix3(

      m11, m12, m13,
      m21, m22, m23,
      m31, m32, m33

      ).Determinant()
      - m01 * Matrix3(

      m10,  m12, m13,
      m20,  m22, m23,
      m30,  m32, m33

      ).Determinant()
      + m02 * Matrix3(

      m10, m11, m13,
      m20, m21, m23,
      m30, m31, m33

      ).Determinant()
      - m03 * Matrix3(

      m10, m11, m12,
      m20, m21, m22,
      m30, m31, m32

      ).Determinant();
  }

}