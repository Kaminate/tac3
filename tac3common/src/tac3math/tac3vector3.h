#pragma once
namespace Tac
{
  struct Vector2;
  struct Vector3
  {
    float x, y, z;
    Vector3(); // does not initialize
    Vector3(float xx, float yy, float zz);
    Vector3(const Vector2 & vec, float zval);
    Vector3(float xx, const Vector2 & yz);
    Vector3 operator-(void) const;  
    Vector3 operator+(const Vector3& rhs) const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3 operator*(const float rhs) const;
    //Vector3 operator*(const Vector3 & rhs) const; // componentwise mult
    Vector3 operator/(const float rhs) const;
    Vector3& operator+=(const Vector3& rhs);
    Vector3& operator-=(const Vector3& rhs);
    Vector3& operator*=(const float rhs);
    Vector3& operator/=(const float rhs);
    bool operator==(const Vector3& rhs) const;
    bool operator!=(const Vector3& rhs) const;
    float & operator[](unsigned index);
    const float & operator[](unsigned index) const;
    float Dot(const Vector3& rhs) const;
    Vector3 Cross(const Vector3& rhs) const;
    float Length(void) const;
    float LengthSq(void) const;
    float Dist(const Vector3 & rhs) const;
    float DistSq(const Vector3 & rhs) const;
    float DistManhattan(const Vector3 & rhs) const;
    void Normalize(void);
    void Zero(void);
    bool IsAlignedWith(const Vector3 & rhs) const;
  };

  template< typename T, unsigned N >
  struct VectorTemplate
  {
    VectorTemplate(){}
    void Zero()
    { 
      for( unsigned i = 0; i < N; ++i )
        vals[ i ] = 0;
    }
    T & operator[]( unsigned index )
    {
      return vals[ index ];
    }
    const T & operator [] ( unsigned index ) const 
    {
      return &vals[ i ];
    }
    T vals[ N ];
  };

  template< typename T >
  void VectorSet( VectorTemplate< T, 3 >& vec, T x, T y, T z )
  {
    vec[ 0 ] = x;
    vec[ 1 ] = y;
    vec[ 2 ] = z;
  }

  typedef VectorTemplate< float, 3 > Vector3f;
  typedef VectorTemplate< unsigned int, 3 > Vector3u;
  typedef VectorTemplate< signed int, 3 > Vector3s; // todo: Vector3s32?

  template< typename T, unsigned N >
  T Dot( 
    const VectorTemplate< T, N >& lhs,
    const VectorTemplate< T, N >& rhs )
  {
    T result( 0 );
    for( unsigned i = 0; i < N; ++i )
      result += lhs[ i ] * rhs[ i ];
    return result;
  }

} //Tac
