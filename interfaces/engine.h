#pragma once
#include <windows.h>
#include <stdint.h>
#include "clientstate.h"
struct player_info_t {
	int64_t __pad0;
	union {
		int64_t xuid;
		struct {
			int xuidlow;
			int xuidhigh;
		};
	};
	char name[ 128 ];
	int userid;
	char guid[ 33 ];
	unsigned int friendsid;
	char friendsname[ 128 ];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[ 4 ];
	unsigned char filesdownloaded;
};



class spatial_query;
class iv_engine_client {
public:

	int get_local_player( ) {
		using original_fn = int( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 12 ]( this );
	}
	int get_player_for_user_id( int user_id ) {
		using original_fn = int( __thiscall* )( iv_engine_client* , int );
		return ( *( original_fn** ) this )[ 9 ]( this , user_id );
	}
	void get_player_info( int index , player_info_t* info ) {
		using original_fn = void( __thiscall* )( iv_engine_client* , int , player_info_t* );
		return ( *( original_fn** ) this )[ 8 ]( this , index , info );
	}
	void get_screen_size( int& width , int& height ) {
		using original_fn = void( __thiscall* )( iv_engine_client* , int& , int& );
		return ( *( original_fn** ) this )[ 5 ]( this , width , height );
	}
	matrix_t& w2s_matrix( ) {
		using original_fn = matrix_t&( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 38 ]( this );
	}

	bool is_in_game( ) {
		using original_fn = bool( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 26 ]( this );
	}
	bool is_connected( ) {
		using original_fn = bool( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 27 ]( this );
	}
	void execute_cmd( const char* cmd ) {
		using original_fn = void( __thiscall* )( iv_engine_client* , const char* );
		return ( *( original_fn** ) this )[ 108 ]( this , cmd ); // this always seems to crash whilst debugging, just feel free to continue.
	}
	void client_cmd( const char* cmd ) {
		using original_fn = void( __thiscall* )( iv_engine_client* , const char* );
		return ( *( original_fn** ) this )[ 7 ]( this , cmd ); // this always seems to crash whilst debugging, just feel free to continue.
	}
	void client_cmd_unrestricted( const char* cmd ) {
		using original_fn = void( __thiscall* )( iv_engine_client* , const char* , bool );
		return ( *( original_fn** ) this )[ 114 ]( this , cmd , false ); // this always seems to crash whilst debugging, just feel free to continue.
	}
	void set_view_angles( vec3_t& angles ) {
		using original_fn = void( __thiscall* )( iv_engine_client* , vec3_t& );
		return ( *( original_fn** ) this )[ 19 ]( this , angles );
	}
	void fire_events( ) {
		using original_fn = void( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 59 ]( this );
	}

	vec3_t get_view_angles( ) {
		vec3_t temp;
		using original_fn = void( __thiscall* )( iv_engine_client* , vec3_t& );
		( *( original_fn** ) this )[ 18 ]( this , temp );
		return temp;
	}

	bool is_taking_screenshot( ) {
		using original_fn = bool( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 92 ]( this );
	}
	spatial_query* bsp_tree_query( ) {
		using original_fn = spatial_query * ( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 43 ]( this );
	}

	bool is_voice_recording( ) {
		using original_fn = bool( __thiscall* )( void* );
		return ( *( original_fn** ) this )[ 224 ]( this );
	}
	bool is_paused( ) {
		using original_fn = bool( __thiscall* )( void* );
		return ( *( original_fn** ) this )[ 90 ]( this );
	}

	template <typename t>
	static t get_that( void* class_pointer , size_t index ) {
		return ( *( t** ) class_pointer )[ index ];
	}

	i_net_channel_info* get_net_channel_info( ) {
		using original_fn = i_net_channel_info * ( __thiscall* )( iv_engine_client* );
		return ( *( original_fn** ) this )[ 78 ]( this );
	}

	net_channel* get_net_channel( )
	{
		typedef net_channel* ( __thiscall* Fn )( void* );
		return get_that<Fn>( this , 78 )( this );
	}
};
