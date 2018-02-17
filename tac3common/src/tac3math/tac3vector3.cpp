#include "tac3vector3.h"
#include "tac3vector2.h"
#include <math.h> //abs
namespace Tac
{

  Vector3::Vector3()
  {

  }

  Vector3::Vector3(float xx, float yy, float zz) : x(xx),y(yy),z(zz)
  {
  }
  Vector3::Vector3(const Vector2 & vec, float zval ) 
    : x(vec.x), y(vec.y), z(zval)
  {
  }
  Vector3::Vector3(float xx, const Vector2 & yz) : x(xx), y(yz.x), z(yz.y)
  {
  }

  Vector3 Vector3::operator-(void) const
  {
    return Vector3(-x, -y, -z);
  }
  Vector3 Vector3::operator+(const Vector3& rhs) const
  {
    return Vector3 (
      x + rhs.x,
      y + rhs.y,
      z + rhs.z);
  }

  Vector3 Vector3::operator-(const Vector3& rhs) const
  {
    return Vector3 (
      x - rhs.x,
      y - rhs.y,
      z - rhs.z);
  }
  Vector3 Vector3::operator*(const float rhs) const
  {
    return Vector3 (
      x * rhs,
      y * rhs,
      z * rhs);
  }

  //Vector3 Vector3::operator*( const Vector3 & rhs ) const
  //{
  //  return Vector3(
  //    x * rhs.x,
  //    y * rhs.y, 
  //    z * rhs.z);
  //}

  Vector3 Vector3::operator/(const float rhs) const
  {
    return Vector3 (
      x / rhs,
      y / rhs,
      z / rhs);
  }
  Vector3& Vector3::operator+=(const Vector3& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }
  Vector3& Vector3::operator-=(const Vector3& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }
  Vector3& Vector3::operator*=(const float rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
  }
  Vector3& Vector3::operator/=(const float rhs)
  {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
  }
  bool Vector3::operator==(const Vector3& rhs) const
  {
    if (abs(x - rhs.x) > 0.001f)
      return false;
    if (abs(y - rhs.y) > 0.001f)
      return false;
    if (abs(z - rhs.z) > 0.001f)
      return false;
    return true;
  }
  bool Vector3::operator!=(const Vector3& rhs) const
  {
    return !(*this == rhs);
  }
  float Vector3::Dot(const Vector3& rhs) const
  {
    return 
      x * rhs.x+
      y * rhs.y+
      z * rhs.z;
  }
  Vector3 Vector3::Cross(const Vector3& rhs) const
  {
    return Vector3(
      y * rhs.z - rhs.y * z,
      z * rhs.x - rhs.z * x,
      x * rhs.y - rhs.x * y);
  }
  float Vector3::Length() const
  {
    return sqrt(x * x + y * y + z * z); 
  }

  float Vector3::LengthSq() const
  {
    return x * x + y * y + z * z; 
  }
  void Vector3::Normalize()
  {
    *this /= Length();
  }
  void Vector3::Zero()
  {
    x = 0;
    y = 0;
    z = 0;
  }

  float & Vector3::operator[]( unsigned index )
  {
    return *(&x + index);
  }

  const float & Vector3::operator[]( unsigned index ) const
  {
    return *(&x + index);
  }

  bool Vector3::IsAlignedWith( const Vector3 & rhs ) const
  {
    return abs(Dot(rhs)) > 1.0f - 0.0001f;
  }

  float Vector3::Dist( const Vector3 & rhs ) const
  {
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    float dz = z - rhs.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
  }

  float Vector3::DistSq( const Vector3 & rhs ) const
  {
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    float dz = z - rhs.z;
    return dx * dx + dy * dy + dz * dz;
  }

  float Vector3::DistManhattan( const Vector3 & rhs ) const
  {
    return abs( rhs.x - x ) + abs( rhs.y - y ) + abs( rhs.z - z );
  }

  Vector3 ComponentwiseMult( const Vector3 & lhs, const Vector3 & rhs )
  {
    return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
  }
}
