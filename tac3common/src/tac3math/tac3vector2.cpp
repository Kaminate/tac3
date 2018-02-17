#include "tac3vector2.h"
#include <math.h> // abs
namespace Tac
{
  Vector2::Vector2()
  {

  }

  Vector2::Vector2(float xx, float yy) : x(xx),y(yy)
  {
  }
  Vector2 Vector2::operator-(void) const
  {
    return Vector2(-x, -y);
  }
  Vector2 Vector2::operator+(const Vector2& rhs) const
  {
    return Vector2 (
      x + rhs.x,
      y + rhs.y);
  }

  Vector2 Vector2::operator-(const Vector2& rhs) const
  {
    return Vector2 (
      x - rhs.x,
      y - rhs.y);
  }
  Vector2 Vector2::operator*(const float rhs) const
  {
    return Vector2 (
      x * rhs,
      y * rhs);
  }

  //Vector2 Vector2::operator*( const Vector2 & rhs ) const
  //{
  //  return Vector2(
  //    x * rhs.x,
  //    y * rhs.y); 
  //}

  Vector2 Vector2::operator/(const float rhs) const
  {
    return Vector2 (
      x / rhs,
      y / rhs);
  }
  Vector2& Vector2::operator+=(const Vector2& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }
  Vector2& Vector2::operator-=(const Vector2& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }
  Vector2& Vector2::operator*=(const float rhs)
  {
    x *= rhs;
    y *= rhs;
    return *this;
  }
  Vector2& Vector2::operator/=(const float rhs)
  {
    x /= rhs;
    y /= rhs;
    return *this;
  }
  bool Vector2::operator==(const Vector2& rhs) const
  {
    if (abs(x - rhs.x) > 0.001f)
      return false;
    if (abs(y - rhs.y) > 0.001f)
      return false;
    return true;
  }
  bool Vector2::operator!=(const Vector2& rhs) const
  {
    return !(*this == rhs);
  }
  float Vector2::Dot(const Vector2& rhs) const
  {
    return x * rhs.x + y * rhs.y;
  }
  float Vector2::Length() const
  {
    return sqrt(x * x + y * y ); 
  }

  float Vector2::LengthSq() const
  {
    return x * x + y * y ; 
  }
  void Vector2::Normalize()
  {
    *this /= Length();
  }
  void Vector2::Zero()
  {
    x = 0;
    y = 0;
  }

  float & Vector2::operator[]( unsigned index )
  {
    return *(&x + index);
  }

  const float & Vector2::operator[]( unsigned index ) const
  {
    return *(&x + index);
  }


  float Vector2::Dist( const Vector2 & rhs ) const
  {
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    return sqrt(dx * dx + dy * dy);
  }

  float Vector2::DistSq( const Vector2 & rhs ) const
  {
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    return dx * dx + dy * dy;
  }

  Vector2 ComponentwiseMult( const Vector2 & lhs, const Vector2 & rhs )
  {
    return Vector2(lhs.x * rhs.x, lhs.y * rhs.y);
  }
}
