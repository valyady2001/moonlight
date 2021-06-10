#pragma once
#include <sdk/defines.h>

class c_surface {
public:
	void unlock_cursor( void ) {
		using unlockcusor_fn = void( __thiscall* )( void* );
		vfunc< unlockcusor_fn >( this , 66 )( this );
	}

	void lock_cursor( void ) {
		using lockcusor_fn = void( __thiscall* )( void* );
		vfunc< lockcusor_fn >( this , 67 )( this );
	}

	void get_cursor_pos( int& x , int& y ) {
		using get_cursor_pos_fn = void( __thiscall* )( void* , int& , int& );
		vfunc< get_cursor_pos_fn >( this , 100 )( this , x , y );
	}

	void play_sound( const char* filename )
	{
		typedef void( __thiscall* orig )( void* , const char* );
		vfunc< orig >( this , 82 )( this , filename );
	}

	void draw_line( int x0 , int y0 , int x1 , int y1 )
	{
		using original_fn = void( __thiscall* )( PVOID , int , int , int , int );
		return ( *( original_fn * * ) this )[ 19 ]( this , x0 , y0 , x1 , y1 );
	}
	vec2_t mouse_pos( ) 
	{
		POINT mousePosition;
		GetCursorPos( &mousePosition );
		ScreenToClient( FindWindow( 0 , "Counter-Strike: Global Offensive" ) , &mousePosition );
		return { static_cast< float >( mousePosition.x ), static_cast< float >( mousePosition.y ) };
	}
};

class i_panel
{
public:
	const char* GetName(unsigned int vguiPanel)
	{
		typedef const char* (__thiscall* tGetName)(void*, unsigned int);
		return vfunc<tGetName>(this, 36)(this, vguiPanel);
	}
};