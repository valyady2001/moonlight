#include "hook.h"
#include <sdk/utils/utilities.h>
#include <thread>
#include <intrin.h>
#include <csgo.h>

template < typename T >
bool hook_address( void* target , void* func , T** original ) {
	if ( !target ) {
#ifdef _DEBUG
		printf( "failed to hook a function for null target\n" );
#endif
		return false;
	}

	MH_STATUS status = MH_UNKNOWN;

	// create hook.
	status = MH_CreateHook( target , func , ( void** ) original );

	if ( status != MH_OK ) {
		interfaces::cvar->console_color_printf( c_color( 255 , 0 , 0 , 255 ) , "[ERROR] " );
		dbg_print( "failed create hook 0x%p ==> 0x%p. status: %s \n" , target , func , MH_StatusToString( status ) );

		return false;
	}

	// enable hook.
	status = MH_EnableHook( target );

	if ( status != MH_OK ) {
		interfaces::cvar->console_color_printf( c_color( 255 , 0 , 0 , 255 ) , "[ERROR] " );
		dbg_print( "failed enable hook 0x%p ==> 0x%p. status: %s \n" , target , func , MH_StatusToString( status ) );

		return false;
	}

	// shush
	//interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[log] " );
	//dbg_print( "hooked 0x%p ==> 0x%p\n" , target , func );

	return true;
}

template < typename T >
bool hook_address( uintptr_t target , void* func , T** original ) {
	return hook_address( reinterpret_cast< void* >( target ) , func , original );
}

namespace hook
{
	void init( )
	{
		const auto model_render_dme = vfunc< decltype( &draw_model_execute ) >( ( void* ) interfaces::model_render , 21 );
		const auto d3d9_end_scene = vfunc< decltype( &end_scene ) >( ( void* ) interfaces::dev , 42 );
		const auto surface_lockcursor = ( void* ) ( utilities::find_sig( "vguimatsurface.dll" , "80 3D ? ? ? ? 00 8B 91 A4 02 00 00 8B 0D ? ? ? ? C6 05 ? ? ? ? 01" ) );
		const auto overrideview = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 83 EC 58 56 57 8B 3D ? ? ? ? 85 FF" ) );
		const auto c_csplayer_get_eye_angles = ( void* ) ( utilities::find_sig( "client.dll" , "56 8B F1 85 F6 74 32" ) );
		const auto c_csplayer_doextraboneprocessing = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 81 EC FC 00 00 00 53 56 8B F1 57" ) );
		const auto buildtransforms = reinterpret_cast< decltype( &build_transformations ) >( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 1C" ) );
		const auto shouldskipanimframe = reinterpret_cast< decltype( &should_skip_animframe ) >( utilities::find_sig( "client.dll" , "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02 5F C3" ) );
		const auto cbaseanim_standardblendingulres = reinterpret_cast< decltype( &standard_blending_rules ) >( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6" ) );
		const auto c_baseanim_updateclientsideanimations = reinterpret_cast< decltype( &update_client_animations ) >( utilities::find_sig( "client.dll" , "8B 0D ? ? ? ? 53 56 57 8B 99 0C 10 00 00 85 DB 74 ? 6A 04 6A 00" ) );
		const auto engine_is_hltv = vfunc< void* >( interfaces::engine , 93 );
		const auto sv_cheats_target = vfunc< decltype( &sv_cheats_get_bool ) >( ( void* ) interfaces::cvar->find_var( "sv_cheats" ) , 13 );
		const auto bspquery_disable_occulusion = vfunc< decltype( &disable_occulusion ) >( ( void* ) interfaces::engine->bsp_tree_query( ) , 6 );
		const auto writeucmd_client = reinterpret_cast< decltype( &write_cmd_delta_buf ) >( utilities::find_sig( "client.dll" , "55 8B EC 83 EC 68 53 56 8B D9 C7 45" ) );
		const auto clmove = reinterpret_cast< decltype( &cl_move ) >( utilities::find_sig( "engine.dll" , "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D ? ? ? ? 8A" ) );
		const auto runcmd = reinterpret_cast< decltype( &run_command ) >( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 C0 83 EC 34 53 56 8B 75 08" ) );
		const auto draw_set_clr = vfunc< decltype( &draw_set_color ) >( ( void* ) interfaces::surface , 15 );
		const auto c_baseanimating_setupbones = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9" ) );
		const auto c_csplayer_calc_view = reinterpret_cast< decltype( &calc_view ) >( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 83 EC 24 53 56 57 FF 75 18" ) );
		const auto player_vtable = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C" ) + 0x45 + 0x2 );
		const auto c_csplayer_weaponshootpos = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83" ) );
		const auto seq_change = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 51 53 8B 5D 08 56 8B F1 57 85" ) );
		const auto levelinitpre = vfunc< decltype( &level_init_pre ) >( ( void* ) interfaces::client , 5 );
		const auto levelend = vfunc< decltype( &level_shutdown ) >( ( void* ) interfaces::client , 7 );
		const auto fireevent = vfunc< decltype( &fire_event ) >( ( void* ) interfaces::engine , 59 );
		const auto cnetchan_sendnetmsg = ( void* ) ( utilities::find_sig( "engine.dll" , "55 8B EC 83 EC 08 56 8B F1 8B 86 ? ? ? ? 85 C0" ) );
		const auto physicssimulate = ( void* ) ( utilities::find_sig( "client.dll" , " 56 8B F1 57 8B BE? ? ? ? 83 EF 01 78 74" ) );
		const auto paint_traverse_target = vfunc< decltype( &paint_traverse ) >( ( void* ) interfaces::panel , 41 );
		const auto gloweffectspectator_target = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 EC 14 53 8B 5D 0C 56 57 85 DB 74" ) );
		const auto camthinkk = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 8B F1 8B 0D ? ? ? ? 57 85 C9" ) );
		const auto camtotp = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 83 EC 0C 8D 55 F4" ) );
		const auto inpred = vfunc< decltype( &in_prediction ) >( ( void* ) interfaces::prediction , 14 );

		MH_Initialize( );
		hook_address( d3d9_end_scene , end_scene , &endscene_fn );
		hook_address( draw_set_clr , draw_set_color , &set_col_fn );
		hook_address( runcmd , run_command , &run_command_fn );
		hook_address( buildtransforms , build_transformations , &transformations_fn );
		hook_address( shouldskipanimframe , should_skip_animframe , &skip_animframe_fn );
		hook_address( c_csplayer_doextraboneprocessing , extra_bone_process , &extra_bone_prc_fn );
		//hook_address( c_baseanimating_setupbones , setup_bones , &setup_bones_fn );
		//hook_address( writeucmd_client , write_cmd_delta_buf , &write_cmd_delta_buf_fn );
		hook_address( sv_cheats_target , sv_cheats_get_bool , &sv_cheats_get_bool_fn );
		hook_address( bspquery_disable_occulusion , disable_occulusion , &model_occulusion_fn );
		//hook_address( c_baseanim_updateclientsideanimations , update_client_animations , &update_client_animations_fn );
		hook_address( engine_is_hltv , is_hltv , &hltv_fn );
		hook_address( cbaseanim_standardblendingulres , standard_blending_rules , &standard_blending_rules_fn );
		hook_address( overrideview , override_view , &override_fn );
		hook_address( model_render_dme , draw_model_execute , &dme_fn );
		hook_address( surface_lockcursor , lock_cursor , &lock_fn );
		hook_address( c_csplayer_calc_view , calc_view , &calc_view_fn );
		hook_address( c_csplayer_weaponshootpos , modify_eye_position , &modify_eye_position_fn );
		hook_address( seq_change , check_for_sequence_change , &check_for_sequence_change_fn );
		hook_address( cnetchan_sendnetmsg , send_netmsg , &send_netmsg_fn );
		//hook_address( levelinitpre , level_init_pre , &level_init_pre_fn );
		//hook_address( levelend , level_shutdown , &level_shutdown_fn );
		hook_address( physicssimulate , physics_simulate , &physics_simulate_fn );
		hook_address( paint_traverse_target , paint_traverse , &paint_traverse_fn );
		hook_address( gloweffectspectator_target , gloweffectspectator , &gloweffectspectator_fn );
		hook_address( clmove , cl_move , &cl_move_fn );
		hook_address( fireevent , fire_event , &fire_event_fn );
		hook_address( inpred , in_prediction , &in_prediction_fn );
		hook_address( c_csplayer_get_eye_angles , get_eye_angles , &eye_angles_fn );
		//hook_address( camthinkk , cam_think , ( void** ) &cam_think_fn );
		//hook_address( camtotp , cam_to_third_person , &cam_to_third_person_fn );

		//const auto rb_clmove = reinterpret_cast< decltype( &csgo::rebuilt_cl_move ) >( utilities::find_sig( "engine.dll" , "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D ? ? ? ? 8A" ) );
		//hook_address( rb_clmove , csgo::rebuilt_cl_move , &csgo::rb_clmove );

		//MH_EnableHook( nullptr );

		//interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[log] " );
		//dbg_print( "Initialized hooks. \n" );
	}

	void init2( )
	{
		const auto create_move_proxy_target = vfunc< decltype( &create_move_proxy ) >( ( void* ) interfaces::client , 22 );
		const auto chclient_fsn = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 8B 0D ? ? ? ? 8B 01 8B 80 74 01 00 00 FF D0 A2" ) );

		hook_address( create_move_proxy_target , create_move_proxy , &create_move_proxy_fn );
		hook_address( chclient_fsn , frame_stage_notify , &fsn_fn );
	}
}
