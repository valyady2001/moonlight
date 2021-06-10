#pragma once
#include <algorithm>
#include <sdk/defines.h>

class c_entlist {
public:
	void* get_client_entity( int index ) {
		using original_fn = void*( __thiscall* )( void* , int );
		return ( *( original_fn** ) this ) [ 3 ]( this , index );
	}

	void* get_by_handle( std::uint32_t h ) {
		using getcliententityfromhandle_fn = void*( __thiscall* )( void* , std::uint32_t );
		return vfunc< getcliententityfromhandle_fn >( this , 4 )( this , h );
	}

	int get_highest_index( ) {
		using original_fn = int( __thiscall* )( c_entlist* );
		return ( *( original_fn** ) this ) [ 6 ]( this );
	}
};