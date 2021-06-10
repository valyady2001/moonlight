#pragma once
#include "vec3.h"
#include <windows.h>
#include "common.h"

#include <interfaces/interfaces.h>
#define pi_value 3.14159265358
#define pi_f		((float)(pi_value))	
#define deg2rad( x  )  ( (float)(x) * (float)(pi_f / 180.f) )
#define rad2deg( x  )  ( (float)(x) * (float)(180.f / pi_f) )

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

namespace math
{
	inline void sin_cos( float radians , float* sine , float* cosine )
	{
		*sine = sin( radians );
		*cosine = cos( radians );
	}

	inline bool w2s( vec3_t& origin , vec2_t& screen )
	{
		const auto screenTransform = [ &origin , &screen ]( ) -> bool
		{
			static std::uintptr_t pViewMatrix;
			if ( !pViewMatrix )
			{
				pViewMatrix = static_cast< std::uintptr_t >( utilities::find_sig( "client.dll" , "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" ) );
				pViewMatrix += 3;
				pViewMatrix = *reinterpret_cast< std::uintptr_t* >( pViewMatrix );
				pViewMatrix += 176;
			}

			const matrix_t& w2sMatrix = *reinterpret_cast< matrix_t* >( pViewMatrix );
			screen.x = w2sMatrix[ 0 ][ 0 ] * origin.x + w2sMatrix[ 0 ][ 1 ] * origin.y + w2sMatrix[ 0 ][ 2 ] * origin.z + w2sMatrix[ 0 ][ 3 ];
			screen.y = w2sMatrix[ 1 ][ 0 ] * origin.x + w2sMatrix[ 1 ][ 1 ] * origin.y + w2sMatrix[ 1 ][ 2 ] * origin.z + w2sMatrix[ 1 ][ 3 ];

			float w = w2sMatrix[ 3 ][ 0 ] * origin.x + w2sMatrix[ 3 ][ 1 ] * origin.y + w2sMatrix[ 3 ][ 2 ] * origin.z + w2sMatrix[ 3 ][ 3 ];

			if ( w < 0.001f )
			{
				screen.x *= 100000;
				screen.y *= 100000;
				return true;
			}

			float invw = 1.f / w;
			screen.x *= invw;
			screen.y *= invw;

			return false;
		};

		if ( !screenTransform( ) )
		{
			int iScreenWidth , iScreenHeight;
			interfaces::engine->get_screen_size( iScreenWidth , iScreenHeight );

			screen.x = ( iScreenWidth * 0.5f ) + ( screen.x * iScreenWidth ) * 0.5f;
			screen.y = ( iScreenHeight * 0.5f ) - ( screen.y * iScreenHeight ) * 0.5f;

			return true;
		}

		return false;
	}
	inline  void VectorTransform( const vec3_t& in , const matrix_t& matrix , vec3_t& out ) {

		out = {
		in.dot( vec3_t( matrix[ 0 ][ 0 ], matrix[ 0 ][ 1 ], matrix[ 0 ][ 2 ] ) ) + matrix[ 0 ][ 3 ],
		in.dot( vec3_t( matrix[ 1 ][ 0 ], matrix[ 1 ][ 1 ], matrix[ 1 ][ 2 ] ) ) + matrix[ 1 ][ 3 ],
		in.dot( vec3_t( matrix[ 2 ][ 0 ], matrix[ 2 ][ 1 ], matrix[ 2 ][ 2 ] ) ) + matrix[ 2 ][ 3 ]
		};
	}
	

	inline void angle_matrix( const vec3_t& angles , matrix_t& matrix )
	{
		float sr , sp , sy , cr , cp , cy;

		sin_cos( deg2rad( angles.y ) , &sy , &cy );
		sin_cos( deg2rad( angles.x ) , &sp , &cp );
		sin_cos( deg2rad( angles.z ) , &sr , &cr );

		// matrix = (YAW * PITCH) * ROLL
		matrix [ 0 ] [ 0 ] = cp * cy;
		matrix [ 1 ] [ 0 ] = cp * sy;
		matrix [ 2 ] [ 0 ] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix [ 0 ] [ 1 ] = sp * srcy - crsy;
		matrix [ 1 ] [ 1 ] = sp * srsy + crcy;
		matrix [ 2 ] [ 1 ] = sr * cp;

		matrix [ 0 ] [ 2 ] = ( sp * crcy + srsy );
		matrix [ 1 ] [ 2 ] = ( sp * crsy - srcy );
		matrix [ 2 ] [ 2 ] = cr * cp;

		matrix [ 0 ] [ 3 ] = 0.0f;
		matrix [ 1 ] [ 3 ] = 0.0f;
		matrix [ 2 ] [ 3 ] = 0.0f;
	}

	inline void matrix_angles( const matrix_t& matrix , vec3_t& angles )
	{
		vec3_t forward , left , up;

		// extract the basis vec3_ts from the matrix. since we only need the z
		// component of the up vec3_t, we don't get x and y.
		forward = { matrix [ 0 ] [ 0 ], matrix [ 1 ] [ 0 ], matrix [ 2 ] [ 0 ] };
		left = { matrix [ 0 ] [ 1 ], matrix [ 1 ] [ 1 ], matrix [ 2 ] [ 1 ] };
		up = { 0.f, 0.f, matrix [ 2 ] [ 2 ] };

		float len = forward.length_2d( );

		// enough here to get angles?
		if ( len > 0.001f ) {
			angles.x = rad2deg( std::atan2( -forward.z , len ) );
			angles.y = rad2deg( std::atan2( forward.y , forward.x ) );
			angles.z = rad2deg( std::atan2( left.z , up.z ) );
		}

		else {
			angles.x = rad2deg( std::atan2( -forward.z , len ) );
			angles.y = rad2deg( std::atan2( -left.x , left.y ) );
			angles.z = 0.f;
		}
	}

	inline void matrix_copy( const matrix_t& in , matrix_t& out )
	{
		std::memcpy( out.base( ) , in.base( ) , sizeof( matrix_t ) );
	}
	inline void AngleMatrix( const vec3_t& ang , const vec3_t& pos , matrix_t& out ) {
		angle_matrix( ang , out );
		out.set_origin( pos );
	}
	inline void concat_transforms( const matrix_t& in1 , const matrix_t& in2 , matrix_t& out )
	{
		if ( &in1 == &out ) {
			matrix_t in1b;
			matrix_copy( in1 , in1b );
			concat_transforms( in1b , in2 , out );
			return;
		}

		if ( &in2 == &out ) {
			matrix_t in2b;
			matrix_copy( in2 , in2b );
			concat_transforms( in1 , in2b , out );
			return;
		}

		out [ 0 ] [ 0 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 0 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 0 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 0 ];
		out [ 0 ] [ 1 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 1 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 1 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 1 ];
		out [ 0 ] [ 2 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 2 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 2 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 2 ];
		out [ 0 ] [ 3 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 3 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 3 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 3 ] + in1 [ 0 ] [ 3 ];

		out [ 1 ] [ 0 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 0 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 0 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 0 ];
		out [ 1 ] [ 1 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 1 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 1 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 1 ];
		out [ 1 ] [ 2 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 2 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 2 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 2 ];
		out [ 1 ] [ 3 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 3 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 3 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 3 ] + in1 [ 1 ] [ 3 ];

		out [ 2 ] [ 0 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 0 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 0 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 0 ];
		out [ 2 ] [ 1 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 1 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 1 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 1 ];
		out [ 2 ] [ 2 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 2 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 2 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 2 ];
		out [ 2 ] [ 3 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 3 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 3 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 3 ] + in1 [ 2 ] [ 3 ];
	}
	

	inline float interpolate( const float from , const float to , const float percent ) {
		return to * percent + from * ( 1.f - percent );
	}

	__forceinline  vec3_t interpolate( const vec3_t from , const vec3_t to , const float percent )
	{
		return to * percent + from * ( 1.f - percent );
	}

	inline float grd_to_bog( float GRD ) {
		return ( pi_value / 180 ) * GRD;
	}

	inline void vector_substract( const vec3_t& a , const vec3_t& b , vec3_t& c )
	{
		CHECK_VALID( a );
		CHECK_VALID( b );
		c.x = a.x - b.x;
		c.y = a.y - b.y;
		c.z = a.z - b.z;
	}

	inline void angle_2vectors( const vec3_t& angles , vec3_t* forward )
	{
		Assert( s_bMathlibInitialized );
		Assert( forward );

		float	sp , sy , cp , cy;

		sy = sin( deg2rad( angles [ 1 ] ) );
		cy = cos( deg2rad( angles [ 1 ] ) );

		sp = sin( deg2rad( angles [ 0 ] ) );
		cp = cos( deg2rad( angles [ 0 ] ) );

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	inline float random_float( float min , float max )
	{
		static auto random_float = reinterpret_cast< float( *)( float , float ) >( GetProcAddress( GetModuleHandleA( "vstdlib.dll" ) , "RandomFloat" ) );

		return random_float( min , max );
	}

	inline void angle_4vec( const vec3_t& angles , vec3_t* forward , vec3_t* right , vec3_t* up )
	{
		static const constexpr auto PIRAD = 0.01745329251f;
		float sr , sp , sy , cr , cp , cy;

		sp = static_cast< float >( sin( double( angles.x ) * PIRAD ) );
		cp = static_cast< float >( cos( double( angles.x ) * PIRAD ) );
		sy = static_cast< float >( sin( double( angles.y ) * PIRAD ) );
		cy = static_cast< float >( cos( double( angles.y ) * PIRAD ) );
		sr = static_cast< float >( sin( double( angles.z ) * PIRAD ) );
		cr = static_cast< float >( cos( double( angles.z ) * PIRAD ) );

		if ( forward )
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if ( right )
		{
			right->x = ( -1 * sr * sp * cy + -1 * cr * -sy );
			right->y = ( -1 * sr * sp * sy + -1 * cr * cy );
			right->z = -1 * sr * cp;
		}

		if ( up )
		{
			up->x = ( cr * sp * cy + -sr * -sy );
			up->y = ( cr * sp * sy + -sr * cy );
			up->z = cr * cp;
		}
	}

	inline void angle_vec( const vec3_t angles , vec3_t& forward , vec3_t& right , vec3_t& up )
	{
		float sp , sy , sr , cp , cy , cr;

		sin_cos( grd_to_bog( angles.x ) , &sp , &cp );
		sin_cos( grd_to_bog( angles.y ) , &sy , &cy );
		sin_cos( grd_to_bog( angles.z ) , &sr , &cr );

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;

		right.x = -1 * sr * sp * cy + -1 * cr * -sy;
		right.y = -1 * sr * sp * sy + -1 * cr * cy;
		right.z = -1 * sr * cp;

		up.x = cr * sp * cy + -sr * -sy;
		up.y = cr * sp * sy + -sr * cy;
		up.z = cr * cp;
	}
	inline vec3_t calculate_angle( const vec3_t& src , const vec3_t& dst )
	{
		vec3_t delta = { dst [ 0 ] - src [ 0 ],dst [ 1 ] - src [ 1 ],dst [ 2 ] - src [ 2 ] };
		float delta_length = sqrt( delta [ 0 ] * delta [ 0 ] + delta [ 1 ] * delta [ 1 ] + delta [ 2 ] * delta [ 2 ] );
		vec3_t final_angle;
		float pitch = -asin( delta [ 2 ] / delta_length ) *( 180 / pi_value );
		float yaw = atan2( delta [ 1 ] , delta [ 0 ] ) *( 180 / pi_value );
		if ( pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180 )
		{
			final_angle [ 0 ] = pitch;
			final_angle [ 1 ] = yaw;
			final_angle [ 2 ] = 0;
		}
		return final_angle;
	}

	static void angle_to_vector( const vec3_t &angles , vec3_t &forward )
	{
		float sp , sy , cp , cy;

		sin_cos( deg2rad( angles.y ) , &sy , &cy );
		sin_cos( deg2rad( angles.x ) , &sp , &cp );

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	inline vec3_t angle_1vec( vec3_t angle ) {
		vec3_t ret;

		float sp , sy , cp , cy;

		sin_cos( deg2rad( angle.y ) , &sy , &cy );
		sin_cos( deg2rad( angle.x ) , &sp , &cp );

		ret.x = cp * cy;
		ret.y = cp * sy;
		ret.z = -sp;

		return ret;
	}


	inline float angle_diff( float destAngle , float srcAngle )
	{
		float delta;

		delta = fmodf( destAngle - srcAngle , 360.0f );
		if ( destAngle > srcAngle )
		{
			if ( delta >= 180 )
				delta -= 360;
		}
		else
		{
			if ( delta <= -180 )
				delta += 360;
		}
		return delta;
	}


	inline vec3_t vector_1angles( vec3_t& forward )
	{
		vec3_t angles;
		float tmp , yaw , pitch;

		if ( forward.x == 0.0f && forward.y == 0.0f ) {
			yaw = 0.0f;

			if ( forward.z > 0.0f )
				pitch = 270.0f;
			else
				pitch = 90.0f;
		}
		else {
			yaw = std::atan2f( forward.y , forward.x ) * 180.0f / pi_value;

			if ( yaw < 0.0f )
				yaw += 360.0f;

			tmp = std::sqrt( forward.x * forward.x + forward.y * forward.y );
			pitch = std::atan2f( -forward.z , tmp ) * 180.0f / pi_value;

			if ( pitch < 0.0f )
				pitch += 360.0f;
		}

		angles.x = pitch;
		angles.y = yaw;
		angles.z = 0.0f;

		return angles;
	}
	inline void vector_angles( const vec3_t &vecForward , vec3_t &vecAngles )
	{
		vec3_t vecView;
		if ( vecForward [ 1 ] == 0.f && vecForward [ 0 ] == 0.f )
		{
			vecView [ 0 ] = 0.f;
			vecView [ 1 ] = 0.f;
		}
		else
		{
			vecView [ 1 ] = atan2( vecForward [ 1 ] , vecForward [ 0 ] ) * 180.f / pi_value;

			if ( vecView [ 1 ] < 0.f )
				vecView [ 1 ] += 360.f;

			vecView [ 2 ] = sqrt( vecForward [ 0 ] * vecForward [ 0 ] + vecForward [ 1 ] * vecForward [ 1 ] );

			vecView [ 0 ] = atan2( vecForward [ 2 ] , vecView [ 2 ] ) * 180.f / pi_value;
		}

		vecAngles [ 0 ] = -vecView [ 0 ];
		vecAngles [ 1 ] = vecView [ 1 ];
		vecAngles [ 2 ] = 0.f;
	}

	inline vec3_t cross_product( const vec3_t &a , const vec3_t &b )
	{
		return vec3_t( a.y*b.z - a.z*b.y , a.z*b.x - a.x*b.z , a.x*b.y - a.y*b.x );
	}
	inline vec3_t vector_approach( vec3_t target , vec3_t value , float speed )
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
	inline void vector_angles3( const vec3_t& forward , vec3_t& up , vec3_t& angles )
	{
		vec3_t left = cross_product( up , forward );
		left.normalize_place( );

		float forwardDist = forward.length_2d( );

		if ( forwardDist > 0.001f )
		{
			angles.x = atan2f( -forward.z , forwardDist ) * 180 / pi_f;
			angles.y = atan2f( forward.y , forward.x ) * 180 / pi_f;

			float upZ = ( left.y * forward.x ) - ( left.x * forward.y );
			angles.z = atan2f( left.z , upZ ) * 180 / pi_f;
		}
		else
		{
			angles.x = atan2f( -forward.z , forwardDist ) * 180 / pi_f;
			angles.y = atan2f( -left.x , left.y ) * 180 / pi_f;
			angles.z = 0;
		}
	}

	inline void normalize_2vec( vec3_t& vIn , vec3_t& vOut )
	{
		float flLen = vIn.length( );
		if ( flLen == 0 ) {
			vOut.init( 0 , 0 , 1 );
			return;
		}
		flLen = 1 / flLen;
		vOut.init( vIn.x * flLen , vIn.y * flLen , vIn.z * flLen );
	}

	inline float normalize_yaw( float yaw )
	{
		if ( yaw > 180 )
		{
			yaw -= ( round( yaw / 360 ) * 360.f );
		}
		else if ( yaw < -180 )
		{
			yaw += ( round( yaw / 360 ) * -360.f );
		}
		return yaw;
	}

	inline float normalize_pitch( float pitch )
	{
		while ( pitch > 89.f )
			pitch -= 180.f;
		while ( pitch < -89.f )
			pitch += 180.f;

		return pitch;
	}

	inline vec3_t normalize_angle( vec3_t angle )
	{
		angle.x = normalize_pitch( angle.x );
		angle.y = normalize_yaw( angle.y );

		if ( ( angle.z > 50 ) || ( angle.z < 50 ) )
			angle.z = 0;

		return angle;
	}

	inline float distance_point_to_line( vec3_t Point , vec3_t LineOrigin , vec3_t Dir ) {
		auto PointDir = Point - LineOrigin;

		auto TempOffset = PointDir.dot( Dir ) / ( Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z );
		if ( TempOffset < 0.000001f )
			return FLT_MAX;

		auto PerpendicularPoint = LineOrigin + ( Dir * TempOffset );

		return ( Point - PerpendicularPoint ).length( );
	}



	inline float get_fov( const vec3_t& view_angle , const vec3_t& aim_angle ) {
		vec3_t ang , aim;

		angle_2vectors( view_angle , &aim );
		angle_2vectors( aim_angle , &ang );

		return rad2deg( acos( aim.dot( ang ) / aim.length_sqr( ) ) );
	}

	inline void normalize_angle2( vec3_t& angles ) {
		for ( auto i = 0; i < 3; i++ ) {
			while ( angles [ i ] < -180.0f ) angles [ i ] += 360.0f;
			while ( angles [ i ] > 180.0f ) angles [ i ] -= 360.0f;
		}
	}

	inline void clamp_angle( vec3_t& angles ) {
		angles.x = std::clamp( angles.x , -89.0f , 89.0f );
		angles.y = std::clamp( angles.y , -180.0f , 180.0f );
		angles.z = 0.0f;
	}

	inline bool sanitize_angle( vec3_t& angles )
	{
		vec3_t temp = angles;
		normalize_angle( temp );
		clamp_angle( temp );

		if ( !isfinite( temp.x ) ||
			!isfinite( temp.y ) ||
			!isfinite( temp.z ) )
			return false;

		angles = temp;

		return true;
	}

	inline float distance( vec2_t point1 , vec2_t point2 )
	{
		float diffY = point1.y - point2.y;
		float diffX = point1.x - point2.x;
		return sqrt( ( diffY * diffY ) + ( diffX * diffX ) );
	}

	inline void AngleMatrix( const vec3_t& angles , matrix_t& matrix )
	{
		float sr , sp , sy , cr , cp , cy;

		sin_cos( deg2rad( angles[ 1 ] ) , &sy , &cy );
		sin_cos( deg2rad( angles[ 0 ] ) , &sp , &cp );
		sin_cos( deg2rad( angles[ 2 ] ) , &sr , &cr );

		// matrix = (YAW * PITCH) * ROLL
		matrix[ 0 ][ 0 ] = cp * cy;
		matrix[ 1 ][ 0 ] = cp * sy;
		matrix[ 2 ][ 0 ] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[ 0 ][ 1 ] = sp * srcy - crsy;
		matrix[ 1 ][ 1 ] = sp * srsy + crcy;
		matrix[ 2 ][ 1 ] = sr * cp;

		matrix[ 0 ][ 2 ] = ( sp * crcy + srsy );
		matrix[ 1 ][ 2 ] = ( sp * crsy - srcy );
		matrix[ 2 ][ 2 ] = cr * cp;

		matrix[ 0 ][ 3 ] = 0.0f;
		matrix[ 1 ][ 3 ] = 0.0f;
		matrix[ 2 ][ 3 ] = 0.0f;
	}

	inline vec3_t vector_rotate( vec3_t& in1 , matrix_t& in2 )
	{
		return vec3_t( in1.dot( in2[ 0 ] ) , in1.dot( in2[ 1 ] ) , in1.dot( in2[ 2 ] ) );
	}

	inline vec3_t vector_rotate( vec3_t& in1 , vec3_t& in2 )
	{
		matrix_t m;
		AngleMatrix( in2 , m );
		return vector_rotate( in1 , m );
	}
	inline float segment_to_segment( const vec3_t s1 , const vec3_t s2 , const vec3_t k1 , const vec3_t k2 )
	{
		static auto constexpr epsilon = 0.00000001;

		auto u = s2 - s1;
		auto v = k2 - k1;
		const auto w = s1 - k1;

		const auto a = u.dot( u );
		const auto b = u.dot( v );
		const auto c = v.dot( v );
		const auto d = u.dot( w );
		const auto e = v.dot( w );
		const auto D = a * c - b * b;
		float sn , sd = D;
		float tn , td = D;

		if ( D < epsilon ) {
			sn = 0.0;
			sd = 1.0;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if ( sn < 0.0 ) {
				sn = 0.0;
				tn = e;
				td = c;
			}
			else if ( sn > sd ) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if ( tn < 0.0 ) {
			tn = 0.0;

			if ( -d < 0.0 )
				sn = 0.0;
			else if ( -d > a )
				sn = sd;
			else {
				sn = -d;
				sd = a;
			}
		}
		else if ( tn > td ) {
			tn = td;

			if ( -d + b < 0.0 )
				sn = 0;
			else if ( -d + b > a )
				sn = sd;
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const float sc = abs( sn ) < epsilon ? 0.0 : sn / sd;
		const float tc = abs( tn ) < epsilon ? 0.0 : tn / td;

		m128 n;
		auto dp = w + u * sc - v * tc;
		n.f[ 0 ] = dp.dot( dp );
		const auto calc = sqrt_ps( n.v );
		return reinterpret_cast< const m128* >( &calc )->f[ 0 ];
	}
	inline void vector_i_transform( const vec3_t& in1 , const matrix_t& in2 , vec3_t& out )
	{
		out.x = ( in1.x - in2[ 0 ][ 3 ] ) * in2[ 0 ][ 0 ] + ( in1.y - in2[ 1 ][ 3 ] ) * in2[ 1 ][ 0 ] + ( in1.z - in2[ 2 ][ 3 ] ) * in2[ 2 ][ 0 ];
		out.y = ( in1.x - in2[ 0 ][ 3 ] ) * in2[ 0 ][ 1 ] + ( in1.y - in2[ 1 ][ 3 ] ) * in2[ 1 ][ 1 ] + ( in1.z - in2[ 2 ][ 3 ] ) * in2[ 2 ][ 1 ];
		out.z = ( in1.x - in2[ 0 ][ 3 ] ) * in2[ 0 ][ 2 ] + ( in1.y - in2[ 1 ][ 3 ] ) * in2[ 1 ][ 2 ] + ( in1.z - in2[ 2 ][ 3 ] ) * in2[ 2 ][ 2 ];
	}

	inline void vector_i_rotate( vec3_t in1 , matrix_t in2 , vec3_t& out )
	{
		out.x = in1.x * in2[ 0 ][ 0 ] + in1.y * in2[ 1 ][ 0 ] + in1.z * in2[ 2 ][ 0 ];
		out.y = in1.x * in2[ 0 ][ 1 ] + in1.y * in2[ 1 ][ 1 ] + in1.z * in2[ 2 ][ 1 ];
		out.z = in1.x * in2[ 0 ][ 2 ] + in1.y * in2[ 1 ][ 2 ] + in1.z * in2[ 2 ][ 2 ];
	}
	inline bool intersect( vec3_t start , vec3_t end , vec3_t a , vec3_t b , float radius )
	{
		const auto dist = segment_to_segment( a , b , start , end );
		return ( dist < radius );
	}

	inline bool intersect_line_with_bb( vec3_t& start , vec3_t& end , vec3_t& min , vec3_t& max ) {
		float d1 , d2 , f;
		auto start_solid = true;
		auto t1 = -1.0f , t2 = 1.0f;

		const float s[ 3 ] = { start.x, start.y, start.z };
		const float e[ 3 ] = { end.x, end.y, end.z };
		const float mi[ 3 ] = { min.x, min.y, min.z };
		const float ma[ 3 ] = { max.x, max.y, max.z };

		for ( auto i = 0; i < 6; i++ ) {
			if ( i >= 3 ) {
				const auto j = i - 3;

				d1 = s[ j ] - ma[ j ];
				d2 = d1 + e[ j ];
			}
			else {
				d1 = -s[ i ] + mi[ i ];
				d2 = d1 - e[ i ];
			}

			if ( d1 > 0.0f && d2 > 0.0f )
				return false;

			if ( d1 <= 0.0f && d2 <= 0.0f )
				continue;

			if ( d1 > 0 )
				start_solid = false;

			if ( d1 > d2 ) {
				f = d1;
				if ( f < 0.0f )
					f = 0.0f;

				f /= d1 - d2;
				if ( f > t1 )
					t1 = f;
			}
			else {
				f = d1 / ( d1 - d2 );
				if ( f < t2 )
					t2 = f;
			}
		}

		return start_solid || ( t1 < t2&& t1 >= 0.0f );
	}

}