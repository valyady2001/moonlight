#include "hook.h"
#include <sdk/utils/utilities.h>
#include <thread>
#include <intrin.h>
#include <csgo.h>
namespace hook
{
	void init( )
	{
		const auto model_render_dme = vfunc< decltype( &draw_model_execute ) >( ( void* ) interfaces::model_render , 21 );
		const auto create_move_proxy_target = vfunc< decltype( &create_move_proxy ) >( ( void* ) interfaces::client , 22 );
		const auto chclient_fsn = ( void* ) ( utilities::find_sig( "client.dll" , "55 8B EC 8B 0D ? ? ? ? 8B 01 8B 80 74 01 00 00 FF D0 A2" ) );
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
		MH_CreateHook( d3d9_end_scene , end_scene , ( void** ) &endscene_fn );
		MH_CreateHook( draw_set_clr , draw_set_color , ( void** ) &set_col_fn );
		MH_CreateHook( runcmd , run_command , ( void** ) &run_command_fn );
		MH_CreateHook( chclient_fsn , frame_stage_notify , ( void** ) &fsn_fn );
		MH_CreateHook( buildtransforms , build_transformations , ( void** ) &transformations_fn );
		MH_CreateHook( shouldskipanimframe , should_skip_animframe , ( void** ) &skip_animframe_fn );
		MH_CreateHook( c_csplayer_doextraboneprocessing , extra_bone_process , ( void** ) &extra_bone_prc_fn );
		//MH_CreateHook( c_baseanimating_setupbones , setup_bones , ( void** ) &setup_bones_fn );
		//MH_CreateHook( writeucmd_client , write_cmd_delta_buf , ( void** ) &write_cmd_delta_buf_fn );
		MH_CreateHook( sv_cheats_target , sv_cheats_get_bool , ( void** ) &sv_cheats_get_bool_fn );
		MH_CreateHook( bspquery_disable_occulusion , disable_occulusion , ( void** ) &model_occulusion_fn );
		//MH_CreateHook( c_baseanim_updateclientsideanimations , update_client_animations , ( void** ) &update_client_animations_fn );
		MH_CreateHook( engine_is_hltv , is_hltv , ( void** ) &hltv_fn );
		MH_CreateHook( cbaseanim_standardblendingulres , standard_blending_rules , ( void** ) &standard_blending_rules_fn );
		MH_CreateHook( overrideview , override_view , ( void** ) &override_fn );
		MH_CreateHook( model_render_dme , draw_model_execute , ( void** ) &dme_fn );
		MH_CreateHook( create_move_proxy_target , create_move_proxy , ( void** ) &create_move_proxy_fn );
		MH_CreateHook( surface_lockcursor , lock_cursor , ( void** ) &lock_fn );
		MH_CreateHook( c_csplayer_calc_view , calc_view , ( void** ) &calc_view_fn );
		MH_CreateHook( c_csplayer_weaponshootpos , modify_eye_position , ( void** ) &modify_eye_position_fn );
		MH_CreateHook( seq_change , check_for_sequence_change , ( void** ) &check_for_sequence_change_fn );
		MH_CreateHook( cnetchan_sendnetmsg , send_netmsg , ( void** ) &send_netmsg_fn );
		//MH_CreateHook( levelinitpre , level_init_pre , ( void** ) &level_init_pre_fn );
		//MH_CreateHook( levelend , level_shutdown , ( void** ) &level_shutdown_fn );
		MH_CreateHook( physicssimulate , physics_simulate , ( void** ) &physics_simulate_fn );
		MH_CreateHook( paint_traverse_target , paint_traverse , ( void** ) &paint_traverse_fn );
		MH_CreateHook( gloweffectspectator_target , gloweffectspectator , ( void** ) &gloweffectspectator_fn );
		MH_CreateHook( clmove , cl_move , ( void** ) &cl_move_fn );
		MH_CreateHook( fireevent , fire_event , ( void** ) &fire_event_fn );
		MH_CreateHook( inpred , in_prediction , ( void** ) &in_prediction_fn );
		MH_CreateHook( c_csplayer_get_eye_angles , get_eye_angles , ( void** ) &eye_angles_fn );
		//MH_CreateHook( camthinkk , cam_think , ( void** ) &cam_think_fn );
		//MH_CreateHook( camtotp , cam_to_third_person , ( void** ) &cam_to_third_person_fn );

		//const auto rb_clmove = reinterpret_cast< decltype( &csgo::rebuilt_cl_move ) >( utilities::find_sig( "engine.dll" , "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D ? ? ? ? 8A" ) );
		//MH_CreateHook( rb_clmove , csgo::rebuilt_cl_move , ( void** ) &csgo::rb_clmove );

		MH_EnableHook( nullptr );

		interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[log] " );
		dbg_print( "Initialized hooks. \n" );
	}
}
