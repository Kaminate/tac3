#include "Tac3Matrix2.h"
#include <iostream>
#include <iomanip>
namespace Tac
{
  Matrix2::Matrix2()
  {
  }
  Matrix2::Matrix2(
    float mm00, float mm01,
    float mm10, float mm11 )
    : m00(mm00), m01(mm01)
    , m10(mm10), m11(mm11)
  {
  }

  Matrix2::Matrix2(
    const Vector2 & col0,
    const Vector2 & col1):
    m00(col0.x), m01(col1.x),
    m10(col0.y), m11(col1.y)
  {

  }

  Vector2 Matrix2::operator*(const Vector2& rhs) const
  {
    return Vector2(
      m00 * rhs.x + m01 * rhs.y,
      m10 * rhs.x + m11 * rhs.y);
  }
  Matrix2 Matrix2::operator*(const Matrix2& rhs) const
  {
    Matrix2 temp;

    temp.m00   =   m00 * rhs.m00   +   m01 * rhs.m10;
    temp.m01   =   m00 * rhs.m01   +   m01 * rhs.m11;

    temp.m10   =   m10 * rhs.m00   +   m11 * rhs.m10;
    temp.m11   =   m10 * rhs.m01   +   m11 * rhs.m11;

    return temp;
  }

  Matrix2 Matrix2::operator*( float val ) const
  {
    return Matrix2(
      m00 * val, m01 * val,
      m10 * val, m11 * val);
  }

  Matrix2& Matrix2::operator*=(const Matrix2& rhs)
  {
    *this = *this * rhs;
    return *this;
  }
  void Matrix2::Zero(void)
  {
    m00 = 0;
    m01 = 0;
    m10 = 0;
    m11 = 0;
  }
  void Matrix2::Identity(void)
  {
    m00 = 1;
    m01 = 0;

    m10 = 0;
    m11 = 1;
  }
  void Matrix2::Transpose(void)
  {
    Matrix2 temp(
      m00, m10,
      m01, m11 );
    *this = temp;
  }

  void Matrix2::Print() const
  {
    for(int row = 0; row < 2; ++row)
    {
      for(int col  = 0; col < 2; ++col)
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

  Matrix2 Matrix2::operator+( const Matrix2 & rhs ) const
  {
    return Matrix2(
      m00 + rhs.m00,
      m01 + rhs.m01,
      m10 + rhs.m10,
      m11 + rhs.m11);
  }

  float * Matrix2::operator[]( int row )
  {
    return &m00 + row * 2;
  }

  const float * Matrix2::operator[]( int row ) const
  {
    return &m00 + row * 2;
  }

  float Matrix2::Determinant() const
  {
    return m00 * m11 - m01 * m10;
  }

} // Tac
