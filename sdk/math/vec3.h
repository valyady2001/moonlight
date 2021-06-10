#pragma once
#include <cmath>
#include <algorithm>

class vec2_t {
public:
	vec2_t( ) = default;
	vec2_t( float x , float y ) {
		this->x = x;
		this->y = y;
	}
	float x , y;

	vec2_t& operator+=( const vec2_t& v ) {
		this->x += v.x; this->y += v.y;

		return *this;
	}
	vec2_t operator+( const vec2_t& v ) const {
		return vec2_t( this->x + v.x , this->y + v.y );
	}
	vec2_t& operator-=( const vec2_t& v ) {
		this->x -= v.x; this->y -= v.y;

		return *this;
	}
	template<typename T>
	vec2_t operator/( const T& v ) const
	{
		return vec2_t( this->x / v , this->y / v );
	}
	vec2_t operator-( const vec2_t& v ) const {
		return vec2_t( this->x - v.x , this->y - v.y );
	}
	bool operator==( const vec2_t& src ) const {
		return ( src.x == this->x ) && ( src.y == y );
	}
};

class vec3_t {
public:
	float x , y , z;

	vec3_t( ) {
		init( );
	}

	vec3_t( float x , float y , float z = 0.0f ) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void init( ) {
		this->x = this->y = this->z = 0.0f;
	}

	void init( float x , float y , float z ) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool is_valid( ) {
		return std::isfinite( this->x ) && std::isfinite( this->y ) && std::isfinite( this->z );
	}

	bool is_zero( ) {
		return vec3_t( this->x , this->y , this->z ) == vec3_t( 0.0f , 0.0f , 0.0f );
	}

	void invalidate( ) {
		this->x = this->y = this->z = std::numeric_limits< float >::infinity( );
	}

	void clear( ) {
		this->x = this->y = this->z = 0.0f;
	}

	float& operator[]( int i ) {
		return ( ( float* ) this )[ i ];
	}
	vec3_t operator+( int i )
	{
		return vec3_t( this->x + i , this->y + i , this->z + i );
	}
	vec3_t operator+( float i )
	{
		return vec3_t( this->x + i , this->y + i , this->z + i );
	}
	float operator[]( int i ) const {
		return ( ( float* ) this )[ i ];
	}

	void zero( ) {
		this->x = this->y = this->z = 0.0f;
	}

	bool operator==( const vec3_t& src ) const {
		return ( src.x == this->x ) && ( src.y == y ) && ( src.z == z );
	}

	bool operator!=( const vec3_t& src ) const {
		return ( src.x != this->x ) || ( src.y != y ) || ( src.z != z );
	}

	vec3_t& operator+=( const vec3_t& v ) {
		this->x += v.x; this->y += v.y; this->z += v.z;

		return *this;
	}

	vec3_t& operator-=( const vec3_t& v ) {
		this->x -= v.x; this->y -= v.y; this->z -= v.z;

		return *this;
	}

	vec3_t& operator*=( float fl ) {
		this->x *= fl;
		this->y *= fl;
		this->z *= fl;

		return *this;
	}

	vec3_t& operator*=( const vec3_t& v ) {
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;

		return *this;
	}

	vec3_t& operator/=( const vec3_t& v ) {
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;

		return *this;
	}

	vec3_t& operator+=( float fl ) {
		this->x += fl;
		this->y += fl;
		this->z += fl;

		return *this;
	}

	vec3_t& operator/=( float fl ) {
		this->x /= fl;
		this->y /= fl;
		this->z /= fl;

		return *this;
	}

	vec3_t& operator-=( float fl ) {
		this->x -= fl;
		this->y -= fl;
		this->z -= fl;

		return *this;
	}
	float* base( )
	{
		return ( float* ) this;
	}

	float const* base( ) const
	{
		return ( float const* ) this;
	}
	void clamp( )
	{
		while ( this->x < -89.0f )
			this->x += 89.0f;

		if ( this->x > 89.0f )
			this->x = 89.0f;

		while ( this->y < -180.0f )
			this->y += 360.0f;

		while ( this->y > 180.0f )
			this->y -= 360.0f;

		this->z = 0.0f;
	}
	void normalize( ) {
		*this = normalized( );
	}

	vec3_t normalized( ) const {
		auto res = *this;
		auto l = res.length( );

		if ( l != 0.0f )
			res /= l;
		else
			res.x = res.y = res.z = 0.0f;

		return res;
	}

	float fnormalize_place( ) {
		float radius = sqrt( x * x + y * y + z * z );

		/* FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero. */
		float iradius = 1.f / ( radius + FLT_EPSILON );

		x *= iradius;
		y *= iradius;
		z *= iradius;

		return radius;
	}

	float normalize_in_place( )
	{
		vec3_t& v = *this;

		float iradius = 1.f / ( this->length( ) + FLT_EPSILON ); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;

		return v.length( );
	}

	void normalize_place( ) {
		auto res = *this;
		auto radius = std::sqrtf( x * x + y * y + z * z );
		auto iradius = 1.0f / ( radius + FLT_EPSILON );

		res.x *= iradius;
		res.y *= iradius;
		res.z *= iradius;
	}

	float dist_to( const vec3_t& vec ) const {
		vec3_t delta;

		delta.x = this->x - vec.x;
		delta.y = this->y - vec.y;
		delta.z = this->z - vec.z;

		return delta.length( );
	}

	float dist_to_sqr( const vec3_t& vec ) const {
		vec3_t delta;

		delta.x = this->x - vec.x;
		delta.y = this->y - vec.y;
		delta.z = this->z - vec.z;

		return delta.length_sqr( );
	}

	float dot_product( const vec3_t& vec ) const {
		return this->x * vec.x + this->y * vec.y + this->z * vec.z;
	}
	float dot( const vec3_t& vOther ) const
	{
		const vec3_t& a = *this;

		return( a.x * vOther.x + a.y * vOther.y + a.z * vOther.z );
	}

	float dot( const float* fOther ) const
	{
		const vec3_t& a = *this;

		return( a.x * fOther[ 0 ] + a.y * fOther[ 1 ] + a.z * fOther[ 2 ] );
	}

	vec3_t cross_product( const vec3_t& vec ) const {
		return vec3_t( this->y * vec.z - this->z * vec.y , this->z * vec.x - this->x * vec.z , this->x * vec.y - this->y * vec.x );
	}


	float length( ) const {
		return std::sqrtf( this->x * this->x + this->y * this->y + this->z * this->z );
	}

	float length_sqr( ) const {
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}

	float length_2d_sqr( ) const {
		return this->x * this->x + this->y * this->y;
	}

	float length_2d( ) const {
		return std::sqrtf( this->x * this->x + this->y * this->y );
	}

	vec3_t& operator=( const vec3_t& vec ) {
		this->x = vec.x; this->y = vec.y; this->z = vec.z;

		return *this;
	}

	vec3_t operator-( ) const {
		return vec3_t( -this->x , -this->y , -this->z );
	}

	vec3_t operator+( const vec3_t& v ) const {
		return vec3_t( this->x + v.x , this->y + v.y , this->z + v.z );
	}

	vec3_t operator-( const vec3_t& v ) const {
		return vec3_t( this->x - v.x , this->y - v.y , this->z - v.z );
	}

	vec3_t operator*( float fl ) const {
		return vec3_t( this->x * fl , this->y * fl , this->z * fl );
	}

	vec3_t operator*( const vec3_t& v ) const {
		return vec3_t( this->x * v.x , this->y * v.y , this->z * v.z );
	}

	vec3_t operator/( float fl ) const {
		return vec3_t( this->x / fl , this->y / fl , this->z / fl );
	}

	vec3_t operator/( const vec3_t& v ) const {
		return vec3_t( this->x / v.x , this->y / v.y , this->z / v.z );
	}
};

__forceinline vec3_t operator*( float lhs , const vec3_t& rhs ) {
	return rhs * lhs;
}

__forceinline vec3_t operator/( float lhs , const vec3_t& rhs ) {
	return rhs / lhs;
}


inline float vec3_t_length( const vec3_t& v )
{
	return ( float ) sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
}


inline vec3_t vec3_t_approach( vec3_t target , vec3_t value , float speed )
{
	vec3_t diff = ( target - value );
	float delta = diff.length( );

	if ( delta > speed )
		value += diff.normalized( ) * speed;
	else if ( delta < -speed )
		value -= diff.normalized( ) * speed;
	else
		value = target;

	return value;
}

class __declspec( align( 16 ) ) vec_alligned : public vec3_t
{

public:

	inline vec_alligned( void ) {};
	inline vec_alligned( float x , float y , float z )
	{
		init( x , y , z );
	}

public:

	explicit vec_alligned( const vec3_t& other )
	{
		init( other.x , other.y , other.z );
	}

	vec_alligned& operator=( const vec3_t& other )
	{
		init( other.x , other.y , other.z );
		return *this;
	}

	vec_alligned& operator=( const vec_alligned& other )
	{
		init( other.x , other.y , other.z );
		return *this;
	}

	float w;
};


class quaternion;
class radian_euler {
public:
	radian_euler( void ) { }
	radian_euler( float x , float y , float z ) { this->x = x; this->y = y; this->z = z; }
	radian_euler( quaternion const& q );
	radian_euler( const vec3_t& angles );

	inline void init( float x = 0.0f , float y = 0.0f , float z = 0.0f ) { this->x = x; this->y = y; this->z = z; }

	vec3_t to_angle( void ) const;
	bool is_valid( ) const;
	void invalidate( );

	inline float* base( ) { return &x; }
	inline const float* base( ) const { return &x; }

	float operator[]( int i ) const;
	float& operator[]( int i );

	float x , y , z;
};

class quaternion
{
public:
	inline quaternion( void ) {}
	inline quaternion( float x , float y , float z , float w ) : x( x ) , y( y ) , z( z ) , w( w ) { }
	inline quaternion( radian_euler const& angle );

	inline void init( float ix = 0.0f , float iy = 0.0f , float iz = 0.0f , float iw = 0.0f ) { x = ix; y = iy; z = iz; w = iw; }

	bool is_valid( ) const;
	void invalidate( );

	bool operator==( const quaternion& src ) const;
	bool operator!=( const quaternion& src ) const;

	float* base( ) { return ( float* ) this; }
	const float* base( ) const { return ( float* ) this; }

	// array access...
	float operator[]( int i ) const;
	float& operator[]( int i );

	float x , y , z , w;
};

class __declspec( align( 16 ) ) quaternion_aligned : public quaternion {
public:
	inline quaternion_aligned( void ) {};
	inline quaternion_aligned( float x , float y , float z , float w )
	{
		init( x , y , z , w );
	}

public:
	explicit quaternion_aligned( const quaternion& other )
	{
		init( other.x , other.y , other.z , other.w );
	}

	quaternion_aligned& operator=( const quaternion& other )
	{
		init( other.x , other.y , other.z , other.w );
		return *this;
	}
};

class matrix_t
{

public:

	matrix_t( ) {}
	matrix_t(
		float m00 , float m01 , float m02 , float m03 ,
		float m10 , float m11 , float m12 , float m13 ,
		float m20 , float m21 , float m22 , float m23 )
	{
		m_values[ 0 ][ 0 ] = m00; m_values[ 0 ][ 1 ] = m01; m_values[ 0 ][ 2 ] = m02; m_values[ 0 ][ 3 ] = m03;
		m_values[ 1 ][ 0 ] = m10; m_values[ 1 ][ 1 ] = m11; m_values[ 1 ][ 2 ] = m12; m_values[ 1 ][ 3 ] = m13;
		m_values[ 2 ][ 0 ] = m20; m_values[ 2 ][ 1 ] = m21; m_values[ 2 ][ 2 ] = m22; m_values[ 2 ][ 3 ] = m23;
	}

	void init( const vec3_t& x , const vec3_t& y , const vec3_t& z , const vec3_t& origin )
	{
		m_values[ 0 ][ 0 ] = x.x; m_values[ 0 ][ 1 ] = y.x; m_values[ 0 ][ 2 ] = z.x; m_values[ 0 ][ 3 ] = origin.x;
		m_values[ 1 ][ 0 ] = x.y; m_values[ 1 ][ 1 ] = y.y; m_values[ 1 ][ 2 ] = z.y; m_values[ 1 ][ 3 ] = origin.y;
		m_values[ 2 ][ 0 ] = x.z; m_values[ 2 ][ 1 ] = y.z; m_values[ 2 ][ 2 ] = z.z; m_values[ 2 ][ 3 ] = origin.z;
	}

	matrix_t( const vec3_t& x , const vec3_t& y , const vec3_t& z , const vec3_t& origin )
	{
		init( x , y , z , origin );
	}

	inline void set_origin( vec3_t const& p )
	{
		m_values[ 0 ][ 3 ] = p.x;
		m_values[ 1 ][ 3 ] = p.y;
		m_values[ 2 ][ 3 ] = p.z;
	}

	inline void invalidate( void )
	{
		for ( int i = 0; i < 3; i++ ) {
			for ( int j = 0; j < 4; j++ ) {
				m_values[ i ][ j ] = std::numeric_limits<float>::infinity( );;
			}
		}
	}

	vec3_t get_x_axis( ) const { return at( 0 ); }
	vec3_t get_y_axis( ) const { return at( 1 ); }
	vec3_t get_z_axis( ) const { return at( 2 ); }
	vec3_t get_origin( ) const { return at( 3 ); }

	vec3_t at( int i ) const {
		return vec3_t{ m_values[ 0 ][ i ], m_values[ 1 ][ i ], m_values[ 2 ][ i ] };
	}

	float* operator[]( int i ) {
		return m_values[ i ];
	}

	const float* operator[]( int i ) const {
		return m_values[ i ];
	}

	float* base( ) {
		return &m_values[ 0 ][ 0 ];
	}

	const float* base( ) const {
		return &m_values[ 0 ][ 0 ];
	}
	float m_values[ 3 ][ 4 ];
};
