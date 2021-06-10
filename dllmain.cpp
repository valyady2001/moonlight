// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <algorithm>
#include <thread>
#include <features.h>
#include "interfaces/interfaces.h"
#include <sdk/utils/netvars.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <features.h>
#include <menu/menu.h>

CCRC gCRC;

int _stdcall init( uintptr_t mod ) {
	while ( !GetModuleHandleA( "serverbrowser.dll" ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	//auto debugbreak = utilities::find_sig( "client.dll" , "CC F3 0F 10 4D ? 0F 57 C0" );

	//DWORD old;
	//VirtualProtect( ( void* ) debugbreak , 1 , PAGE_EXECUTE_READWRITE , &old );

	//*( uint8_t* ) debugbreak = 0x90;

	//VirtualProtect( ( void* ) debugbreak , 1 , old , &old );
	try {
		interfaces::init( );
		netvar_manager::netvar_hook_manager.Initialize( );
		features::chams::init( );
		menu::initialize( );
		hook::init( );
		features::events::events.setup( );

		interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[log] " );
		dbg_print( "Cheat fully injected. \n" );
	}
	catch ( const std::runtime_error& error ) {
		MessageBoxA( NULL , error.what( ) , "DEBUG" , MB_OK | MB_ICONERROR );
		menu::shutdown( );
		MH_RemoveHook( MH_ALL_HOOKS );
		MH_Uninitialize( );
		if ( g::local )
			g::local->client_animations( ) = true;

		features::events::events.release( );

		FreeLibraryAndExitThread( HMODULE( mod ) , 0 );
	}

	while ( !GetAsyncKeyState( VK_END ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

#ifdef debug
	fclose( ( FILE* ) stdin );
	fclose( ( FILE* ) stdout );
	FreeConsole( );
#endif

	menu::shutdown( );
	MH_RemoveHook( MH_ALL_HOOKS );
	MH_Uninitialize( );
	if ( g::local )
		g::local->client_animations( ) = true;

	features::events::events.release( );

	FreeLibraryAndExitThread( HMODULE( mod ) , 0 );
}

int __stdcall DllMain( HINSTANCE inst , std::uint32_t reason , void* reserved ) {
	if ( reason == 1 )
		CreateThread( nullptr , 0 , LPTHREAD_START_ROUTINE( init ) , HMODULE( inst ) , 0 , nullptr );

	return 1;
}