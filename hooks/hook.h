#pragma once
#include "minhook.h"
#include <cstdint>
#include <interfaces/interfaces.h>
#include <sdk/defines.h>
#include <sdk/globals.h>
#include <deque>

namespace hook
{
	using physics_fn = void( __fastcall* )( base_entity* );
	long __fastcall end_scene( void* edx , void* ecx , IDirect3DDevice9* device );
	void __fastcall create_move_proxy( void* _this , int , int sequence_number , float input_sample_frametime , bool active );
	void __stdcall create_move( int sequence_number , float input_sample_frametime , bool active , bool& bSendPacket );
	void __fastcall lock_cursor( void* ecx , void* edx );
	void __fastcall override_view( void* ecx , void* edx , view_setup* setup );
	void __fastcall extra_bone_process( void* ecx , void* edx , int a2 , int a3 , int a4 , int a5 , int a6 , int a7 );
	vec3_t* __fastcall get_eye_angles( void* ecx , void* edx );
	void __fastcall frame_stage_notify( void* ecx , void* edx , int stage );
	void __fastcall build_transformations( void* ecx , void* edx , studiohdr_t* hdr , vec3_t* pos , quaternion* q , const matrix_t& transform , int mask , uint8_t* computed );
	void __fastcall update_client_animations( void* ecx , void* edx );
	bool __fastcall is_hltv( void* ecx , void* edx );
	void __fastcall cam_think( vinput* ecx , void* edx );
	void __fastcall cam_to_third_person( vinput* ecx , void* edx );
	bool __fastcall sv_cheats_get_bool( void* ecx , void* edx );
	void __fastcall physics_simulate( void* ecx , void* edx );
	bool __fastcall in_prediction( void* ecx , void* edx );
	int __fastcall disable_occulusion( void* bsp , void* edx , vec3_t& mins , vec3_t& maxs , unsigned short* pList , int listMax );
	bool __fastcall write_cmd_delta_buf( void* edx , void* ecx , int slot , bf_write* buf , int from , int to , bool new_command );
	void __cdecl cl_move( float accumulated_extra_samples , bool bFinalTick );
	void __fastcall run_command( void* ecx , void* edx , base_entity* pl , usercmd* cmd , void* move_helper );
	void __fastcall draw_set_color( c_surface* ecx , void* edx , int r , int g , int b , int a );
	void __fastcall calc_view( void* ecx , void* edx , vec3_t& eye_origin , vec3_t& eye_angles , float& z_near , float& z_far , float& fov );
	void __fastcall modify_eye_position( void* this_pointer , void* edx , vec3_t& input_eye_position );
	void __fastcall check_for_sequence_change( void* this_pointer , void* edx , void* hdr , int cur_sequence , bool force_new_sequence , bool interpolate );
	void __stdcall level_init_pre( const char* map );
	void __stdcall level_shutdown( );
	bool __fastcall send_netmsg( net_channel* pNetChan , void* edx , i_net_message& msg , bool bForceReliable , bool bVoice );
	bool __fastcall should_skip_animframe( void* ecx , void* edx );
	bool __fastcall setup_bones( void* ecx , void* edx , matrix_t* out , int max_bones , int mask , float curtime );
	void __fastcall standard_blending_rules( void* ecx , void* edx , studiohdr_t* hdr , vec3_t* pos , quaternion* q , float curtime , int mask );
	void __fastcall draw_model_execute( void* ecx , void* edx , void* ctx , void* state , const mdlrender_info_t& info , matrix_t* bone_to_world );
	void __fastcall paint_traverse( void* ecx , void* edx , unsigned int panel , bool ForceRepaint , bool AllowForce );
	bool __cdecl gloweffectspectator( void* ecx , void* edx , int& GlowStyle , vec3_t& GlowColor , float& AlphaStart , float& Alpha , float& TimeStart , float& TimeTarget , bool& Animate );

	bool __fastcall fire_event( void* ecx , void* edx );

	inline decltype( &cam_to_third_person ) cam_to_third_person_fn;
	inline decltype( &end_scene ) endscene_fn;
	inline decltype( &cam_think ) cam_think_fn;
	inline decltype( &in_prediction ) in_prediction_fn;
	inline decltype( &fire_event ) fire_event_fn;
	inline decltype( &send_netmsg ) send_netmsg_fn;
	inline decltype( &frame_stage_notify ) fsn_fn;
	inline decltype( &draw_model_execute ) dme_fn;
	inline decltype( &create_move_proxy ) create_move_proxy_fn;
	inline decltype( &lock_cursor ) lock_fn;
	inline decltype( &override_view ) override_fn;
	inline decltype( &extra_bone_process ) extra_bone_prc_fn;
	inline decltype( &build_transformations ) transformations_fn;
	inline decltype( &should_skip_animframe ) skip_animframe_fn;
	inline decltype( &standard_blending_rules ) standard_blending_rules_fn;
	inline decltype( &update_client_animations ) update_client_animations_fn;
	inline decltype( &is_hltv ) hltv_fn;
	inline decltype( &sv_cheats_get_bool ) sv_cheats_get_bool_fn;
	inline decltype( &disable_occulusion ) model_occulusion_fn;
	inline decltype( &get_eye_angles ) eye_angles_fn;
	inline decltype( &write_cmd_delta_buf ) write_cmd_delta_buf_fn;
	inline decltype( &cl_move ) cl_move_fn;
	inline decltype( &run_command ) run_command_fn;
	inline decltype( &draw_set_color ) set_col_fn;
	inline decltype( &setup_bones ) setup_bones_fn;
	inline decltype( &calc_view ) calc_view_fn;
	inline decltype( &modify_eye_position ) modify_eye_position_fn;
	inline decltype( &check_for_sequence_change ) check_for_sequence_change_fn;
	inline decltype( &level_init_pre ) level_init_pre_fn;
	inline decltype( &level_shutdown ) level_shutdown_fn;
	inline decltype( &paint_traverse ) paint_traverse_fn;
	inline decltype( &gloweffectspectator ) gloweffectspectator_fn;
	inline decltype( &physics_simulate ) physics_simulate_fn;

	void init( );
	void init2( );
}

