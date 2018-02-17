#pragma once
namespace Tac
{
  struct Vector2
  {
    float x, y;
    Vector2(); // does not initialize
    Vector2(float xx, float yy);
    Vector2 operator-() const;  
    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2 operator*(const float rhs) const;
    //Vector2 operator*(const Vector2 & rhs) const; // componentwise mult
    Vector2 operator/(const float rhs) const;
    Vector2& operator+=(const Vector2& rhs);
    Vector2& operator-=(const Vector2& rhs);
    Vector2& operator*=(const float rhs);
    Vector2& operator/=(const float rhs);
    bool operator==(const Vector2& rhs) const;
    bool operator!=(const Vector2& rhs) const;
    float & operator[](unsigned index);
    const float & operator[](unsigned index) const;
    float Dot(const Vector2& rhs) const;
    float Length() const;
    float LengthSq() const;
    float Dist(const Vector2 & rhs) const;
    float DistSq(const Vector2 & rhs) const;
    void Normalize();
    void Zero();
  };
}
