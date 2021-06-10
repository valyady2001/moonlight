#include "interfaces.h"
#include <utilities.h>
#include <fstream>

void* capture_interface( const char* mod , const char* iface ) {
	using fn_capture_iface_t = void* ( * )( const char* , int* );
	auto fn_addr = ( fn_capture_iface_t ) GetProcAddress( GetModuleHandleA( mod ) , "CreateInterface" );

	auto iface_addr = fn_addr( iface , nullptr );
	printf( "found %s at 0x%p\n" , iface , fn_addr( iface , nullptr ) );

	return iface_addr;
}


template< class T >
T* get_interface( std::string szModuleName , std::string szInterfaceName , bool bSkip = false )
{
	if ( szModuleName.empty( ) || szInterfaceName.empty( ) )
		return nullptr;
	typedef PVOID( *CreateInterfaceFn )( const char* pszName , int* piReturnCode );
	CreateInterfaceFn hInterface = nullptr;
	while ( !hInterface )
	{
		hInterface = ( CreateInterfaceFn ) GetProcAddress( GetModuleHandleA( szModuleName.c_str( ) ) , "CreateInterface" );
		Sleep( 5 );
	}

	char pszBuffer[ 256 ];
	for ( int i = 0; i < 100; i++ )
	{
		sprintf_s( pszBuffer , "%s%0.3d" , szInterfaceName.c_str( ) , i );
		PVOID pInterface = hInterface( pszBuffer , nullptr );

		if ( pInterface && pInterface != NULL )
		{
			if ( bSkip )
				sprintf_s( pszBuffer , "%s%0.3d" , szInterfaceName.c_str( ) , i + 1 );

			Sleep( 5 );
			break;
		}
	}

	return ( T* ) hInterface( pszBuffer , nullptr );
}

DWORD interfaces::dev;
iv_engine_client* interfaces::engine = nullptr;
i_input_system* interfaces::input_system = nullptr;
c_input* interfaces::input = nullptr;
vinput* interfaces::input2 = nullptr;
player_prediction* interfaces::prediction = nullptr;
player_move_helper* interfaces::move_helper = nullptr;
player_game_movement* interfaces::game_movement = nullptr;
clientstate* interfaces::client_state = nullptr;
trace* interfaces::trace_ray = nullptr;
physic_props* interfaces::physics_props = nullptr;
c_entlist* interfaces::ent_list = nullptr;
c_client* interfaces::client = nullptr;
c_global_vars* interfaces::global_vars = nullptr;
c_mdlrender* interfaces::model_render = nullptr;
c_mdlinfo* interfaces::model_info = nullptr;
c_matsys* interfaces::material_system = nullptr;
c_surface* interfaces::surface = nullptr;
c_cvar* interfaces::cvar = nullptr;
i_game_event_manager2* interfaces::event_manager = nullptr;
c_mem_alloc* interfaces::mem_alloc = nullptr;
c_clientmode* interfaces::clientmode = nullptr;
c_glow_manager* interfaces::glow_manager = nullptr;
i_engine_sound* interfaces::sound = nullptr;
player_resource* interfaces::ent_resource = nullptr;
IViewRenderBeams* interfaces::render_beams = nullptr;
i_render_view* interfaces::render_view = nullptr;
i_panel* interfaces::panel = nullptr;
iv_debug_overlay* interfaces::debug_overlay = nullptr;

size_t interfaces::PlayerAnimState = 0;

static auto CTraceFilterSimple_vmt = ( void* ) ( *( uintptr_t* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F0 83 EC 7C 56 52" ) + 0x3D ));

trace_filter_simple_game_t::trace_filter_simple_game_t( ) :
	m_vmt{ CTraceFilterSimple_vmt } ,
	m_pass_ent1{ } ,
	m_collision_group{ } ,
	m_shouldhit_check_fn{ }
{
}

trace_filter_simple_game_t::trace_filter_simple_game_t( const base_entity* pass_ent1 , int collision_group , should_hit_func_t shouldhit_check_fn ) :
	m_vmt{ CTraceFilterSimple_vmt } ,
	m_pass_ent1{ pass_ent1 } ,
	m_collision_group{ collision_group } ,
	m_shouldhit_check_fn{ shouldhit_check_fn }
{
}

trace_filter_skip_two_entities_game_t::trace_filter_skip_two_entities_game_t( ) :
	m_vmt{ CTraceFilterSimple_vmt } ,
	m_pass_ent1{ } ,
	m_collision_group{ } ,
	m_shouldhit_check_fn{ } ,
	m_pass_ent2{ }
{
}

trace_filter_skip_two_entities_game_t::trace_filter_skip_two_entities_game_t( const base_entity* pass_ent1 , const base_entity* pass_ent2 , int collision_group , should_hit_func_t shouldhit_check_fn ) :
	m_vmt{ CTraceFilterSimple_vmt } ,
	m_pass_ent1{ pass_ent1 } ,
	m_collision_group{ collision_group } ,
	m_shouldhit_check_fn{ shouldhit_check_fn } ,
	m_pass_ent2{ pass_ent2 }
{
}

namespace interfaces
{
	void init( )
	{
		dev = **( DWORD** ) ( utilities::find_sig( "shaderapidx9.dll" , "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C" ) + 1 );
		engine = get_interface<iv_engine_client>( "engine.dll" , "VEngineClient" );
		input_system = get_interface<i_input_system>( "inputsystem.dll" , "InputSystemVersion" );
		input = *( c_input** ) ( utilities::find_sig( "client.dll" , "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) + 1 );
		input2 = *( vinput** ) ( utilities::find_sig( "client.dll" , "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) + 1 );
		game_movement = get_interface<player_game_movement>( "client.dll" , "GameMovement" );
		prediction = get_interface<player_prediction>( "client.dll" , "VClientPrediction" );
		move_helper = **( player_move_helper*** ) ( utilities::find_sig( "client.dll" , "8B 0D ? ? ? ? 8B 46 08 68" ) + 2 );
		client_state = **( clientstate*** ) ( utilities::find_sig( "engine.dll" , "A1 ? ? ? ? 8B 80 ? ? ? ? C3" ) + 1 );
		trace_ray = get_interface<trace>( "engine.dll" , "EngineTraceClient" );
		physics_props = get_interface<physic_props>( "vphysics.dll" , "VPhysicsSurfaceProps" );
		ent_list = get_interface<c_entlist>( "client.dll" , "VClientEntityList" );
		client = get_interface<c_client>( "client.dll" , "VClient" );
		global_vars = **reinterpret_cast< c_global_vars*** >( ( *reinterpret_cast< uintptr_t** >( client ) )[ 11 ] + 10 );
		model_render = get_interface<c_mdlrender>( "engine.dll" , "VEngineModel" );
		model_info = get_interface<c_mdlinfo>( "engine.dll" , "VModelInfoClient" );
		material_system = get_interface<c_matsys>( "materialsystem.dll" , "VMaterialSystem" );
		surface = get_interface< c_surface>( "vguimatsurface.dll" , "VGUI_Surface" );
		cvar = get_interface< c_cvar>( "vstdlib.dll" , "VEngineCvar" );
		render_view = reinterpret_cast< i_render_view* >( capture_interface( "engine.dll" , "VEngineRenderView014" ) );
		event_manager = reinterpret_cast< i_game_event_manager2* >( capture_interface( "engine.dll" , "GAMEEVENTSMANAGER002" ) );
		mem_alloc = *reinterpret_cast< c_mem_alloc** > ( GetProcAddress( GetModuleHandleA( "tier0.dll" ) , "g_pMemAlloc" ) );
		clientmode = **reinterpret_cast< c_clientmode*** >( ( *reinterpret_cast< uintptr_t** >( client ) )[ 10 ] + 5 );
		glow_manager = ( c_glow_manager* ) ( *( uintptr_t* ) ( utilities::find_sig( ( "client.dll" ) , "0F 11 05 ? ? ? ? 83 C8 01 C7 05 ? ? ? ? 00 00 00 00" ) + 3 ) );
		sound = get_interface<i_engine_sound>( "engine.dll" , "IEngineSoundClient" );
		render_beams = *reinterpret_cast< IViewRenderBeams** >( utilities::find_sig( "client.dll" , "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9" ) + 1 );
		debug_overlay = get_interface<iv_debug_overlay>( "engine.dll" , "VDebugOverlay" );
		ent_resource = *( player_resource** ) ( utilities::find_sig( "client.dll" , "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7" ) );
		PlayerAnimState = *( size_t* ) ( utilities::find_sig( "client.dll" , "8B 8E ? ? ? ? 85 C9 74 3E" ) + 2 );

		panel = get_interface< i_panel>( "vgui2.dll" , "VGUI_Panel" );


		interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[log] " );
		dbg_print( "Initialized interfaces.\n" );
	}
}
