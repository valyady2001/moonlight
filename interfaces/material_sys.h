#pragma once
#include <sdk/defines.h>
#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

using material_handle_t = unsigned short;

enum material_vars_t : int
{
	material_var_debug = ( 1 << 0 ) ,
	material_var_no_debug_override = ( 1 << 1 ) ,
	material_var_no_draw = ( 1 << 2 ) ,
	material_var_use_in_fillrate_mode = ( 1 << 3 ) ,
	material_var_vertexcolor = ( 1 << 4 ) ,
	material_var_vertexalpha = ( 1 << 5 ) ,
	material_var_selfillum = ( 1 << 6 ) ,
	material_var_additive = ( 1 << 7 ) ,
	material_var_alphatest = ( 1 << 8 ) ,
	material_var_znearer = ( 1 << 10 ) ,
	material_var_model = ( 1 << 11 ) ,
	material_var_flat = ( 1 << 12 ) ,
	material_var_nocull = ( 1 << 13 ) ,
	material_var_nofog = ( 1 << 14 ) ,
	material_var_ignorez = ( 1 << 15 ) ,
	material_var_decal = ( 1 << 16 ) ,
	material_var_envmapsphere = ( 1 << 17 ) ,
	material_var_envmapcameraspace = ( 1 << 19 ) ,
	material_var_basealphaenvmapmask = ( 1 << 20 ) ,
	material_var_translucent = ( 1 << 21 ) ,
	material_var_normalmapalphaenvmapmask = ( 1 << 22 ) ,
	material_var_needs_software_skinning = ( 1 << 23 ) ,
	material_var_opaquetexture = ( 1 << 24 ) ,
	material_var_envmapmode = ( 1 << 25 ) ,
	material_var_suppress_decals = ( 1 << 26 ) ,
	material_var_halflambert = ( 1 << 27 ) ,
	material_var_wireframe = ( 1 << 28 ) ,
	material_var_allowalphatocoverage = ( 1 << 29 ) ,
	material_var_alpha_modified_by_proxy = ( 1 << 30 ) ,
	material_var_vertexfog = ( 1 << 31 ) ,
};

class material_var_t {
public:

	void set_vec3_t( const float x , const float y , const float z ) {
		using set_vec3_t_fn = void( __thiscall* )( void* , float , float , float );
		return vfunc< set_vec3_t_fn >( this , 11 )( this , x , y , z );
	}

};

typedef unsigned short MaterialHandle_t;
DECLARE_POINTER_HANDLE( MaterialLock_t );

class material_t {
public:
	const char* get_name( void ) {
		using getname_fn = const char* ( __thiscall* )( void* );
		return vfunc< getname_fn >( this , 0 )( this );
	}

	const char* get_texture_group_name( void ) {
		using gettexturegroupname_fn = const char* ( __thiscall* )( void* );
		return vfunc< gettexturegroupname_fn >( this , 1 )( this );
	}

	void increment_reference_count( void ) {
		using increferencecount_fn = void( __thiscall* )( void* );
		vfunc< increferencecount_fn >( this , 12 )( this );
	}


	material_var_t* find_var( const char* var , bool* found , bool complain = true ) {
		using findvar_fn = material_var_t * ( __thiscall* )( void* , const char* , bool* , bool );
		return vfunc< findvar_fn >( this , 11 )( this , var , found , complain );
	}

	void alpha_modulate( int a ) {
		using alpha_modulate_fn = void( __thiscall* )( void* , float );
		vfunc< alpha_modulate_fn >( this , 27 )( this , static_cast< float >( a ) / 255.0f );
	}

	void color_modulate( int r , int g , int b ) {
		using color_modulate_fn = void( __thiscall* )( void* , float , float , float );
		vfunc< color_modulate_fn >( this , 28 )( this , static_cast< float >( r ) / 255.0f , static_cast< float >( g ) / 255.0f , static_cast< float >( b ) / 255.0f );
	}

	void alpha_modulate( float a ) {
		using alpha_modulate_fn = void( __thiscall* )( void* , float );
		vfunc< alpha_modulate_fn >( this , 27 )( this , a );
	}

	void color_modulate( float r , float g , float b ) {
		using color_modulate_fn = void( __thiscall* )( void* , float , float , float );
		vfunc< color_modulate_fn >( this , 28 )( this , r , g , b );
	}

	void set_material_var_flag( std::uint32_t flag , bool state ) {
		using setmaterialvarflag_fn = void( __thiscall* )( void* , std::uint32_t , bool );
		vfunc< setmaterialvarflag_fn >( this , 29 )( this , flag , state );
	}
};


class c_matsys {
public:
	material_t* create_material( const char* name , void* key_values ) {
		using creatematerial_fn = material_t * ( __thiscall* )( void* , const char* , void* );
		return vfunc< creatematerial_fn >( this , 83 )( this , name , key_values );
	}

	material_handle_t first_material( ) {
		using original_fn = material_handle_t( __thiscall* )( c_matsys* );
		return ( *( original_fn** ) this )[ 86 ]( this );
	}
	material_handle_t next_material( material_handle_t handle ) {
		using original_fn = material_handle_t( __thiscall* )( c_matsys* , material_handle_t );
		return ( *( original_fn** ) this )[ 87 ]( this , handle );
	}
	material_handle_t invalid_material_handle( ) {
		using original_fn = material_handle_t( __thiscall* )( c_matsys* );
		return ( *( original_fn** ) this )[ 88 ]( this );
	}
	material_t* get_material( material_handle_t handle ) {
		using original_fn = material_t * ( __thiscall* )( c_matsys* , material_handle_t );
		return ( *( original_fn** ) this )[ 89 ]( this , handle );
	}

	material_t* find_material( const char* name , const char* group ) {
		using creatematerial_fn = material_t * ( __thiscall* )( void* , const char* , const char* );
		return vfunc< creatematerial_fn >( this , 84 )( this , name , group );
	}

};